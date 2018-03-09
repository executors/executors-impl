#include <experimental/execution>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

namespace execution = std::experimental::execution;

//------------------------------------------------------------------------------

// An executor that launches a new thread for each function submitted to it.
class new_thread_executor
{
public:
  // Singleton execution context that manages threads launched by the new_thread_executor.
  class thread_bag
  {
    friend class new_thread_executor;

    void add_thread(std::thread&& t)
    {
      std::unique_lock<std::mutex> lock(mutex_);
      threads_.push_back(std::move(t));
    }

    thread_bag() = default;

    ~thread_bag()
    {
      for (auto& t : threads_)
        t.join();
    }

    std::mutex mutex_;
    std::vector<std::thread> threads_;
  };

  static thread_bag& query(execution::context_t)
  {
    static thread_bag threads;
    return threads;
  }

  static bool query(execution::never_blocking_t)
  {
    return true;
  }

  template <class Func>
  void execute(Func f) const
  {
    thread_bag& bag = query(execution::context);
    bag.add_thread(std::thread(std::move(f)));
  }

  friend bool operator==(const new_thread_executor&, const new_thread_executor&) noexcept
  {
    return true;
  }

  friend bool operator!=(const new_thread_executor&, const new_thread_executor&) noexcept
  {
    return false;
  }
};

//------------------------------------------------------------------------------

// Helper trait to determine the argument types of a function or function object.
template <class T>
struct args_tuple
{
  using type = typename args_tuple<decltype(&T::operator())>::type;
};

template <class R, class C, class... Args>
struct args_tuple<R(C::*)(Args...)>
{
  using type = std::tuple<Args...>;
};

template <class R, class C, class... Args>
struct args_tuple<R(C::*)(Args...) const>
{
  using type = std::tuple<Args...>;
};

template <class R, class... Args>
struct args_tuple<R(*)(Args...)>
{
  using type = std::tuple<Args...>;
};

template <class T>
using args_tuple_t = typename args_tuple<T>::type;

//------------------------------------------------------------------------------

template <class T, class DefaultExecutor>
auto get_associated_executor_impl(const T& t, const DefaultExecutor&, int)
  -> decltype(t.get_executor())
{
  return t.get_executor();
}

template <class T, class DefaultExecutor>
DefaultExecutor get_associated_executor_impl(const T&, const DefaultExecutor& dflt, ...)
{
  return dflt;
}

// Determine an object's associated executor.
template <class T, class DefaultExecutor>
auto get_associated_executor(const T& t, const DefaultExecutor& dflt)
{
  return get_associated_executor_impl(t, dflt, 0);
}

//------------------------------------------------------------------------------

template <class Func, class ArgTuple = args_tuple_t<Func>>
class executor_binder_base;

template <class Func, class... Args>
class executor_binder_base<Func, std::tuple<Args...>>
{
public:
  executor_binder_base(Func f)
    : function_(std::move(f))
  {
  }

  auto operator()(Args... args)
  {
    return function_(std::forward<Args>(args)...);
  }

private:
  Func function_;
};

template <class Executor, class Func>
class executor_binder : public executor_binder_base<Func>
{
public:
  executor_binder(const Executor& ex, Func f)
    : executor_binder_base<Func>(std::move(f)), executor_(ex)
  {
  }

  Executor get_executor() const noexcept
  {
    return executor_;
  }

private:
  Executor executor_;
  Func function_;
};

// Bind an executor to an object as its associated executor.
template <class Executor, class Func>
executor_binder<Executor, Func> bind_executor(const Executor& ex, Func f) noexcept
{
  return {ex, std::move(f)};
}

//------------------------------------------------------------------------------

// Base class for all thread-safe queue implementations.
class queue_impl_base
{
  template <class> friend class queue_front;
  template <class> friend class queue_back;
  std::mutex mutex_;
  std::condition_variable condition_;
  bool stop_ = false;
};

// Underlying implementation of a thread-safe queue, shared between the
// queue_front and queue_back classes.
template <class T>
class queue_impl : public queue_impl_base
{
  template <class> friend class queue_front;
  template <class> friend class queue_back;
  std::queue<T> queue_;
};

// The front end of a queue between consecutive pipeline stages.
template <class T>
class queue_front
{
public:
  using value_type = T;

  explicit queue_front(std::shared_ptr<queue_impl<T>> impl)
    : impl_(impl)
  {
  }

  void push(T t)
  {
    std::unique_lock<std::mutex> lock(impl_->mutex_);
    impl_->queue_.push(std::move(t));
    impl_->condition_.notify_one();
  }

  void stop()
  {
    std::unique_lock<std::mutex> lock(impl_->mutex_);
    impl_->stop_ = true;
    impl_->condition_.notify_one();
  }

private:
  std::shared_ptr<queue_impl<T>> impl_;
};

// The back end of a queue between consecutive pipeline stages.
template <class T>
class queue_back
{
public:
  using value_type = T;

  explicit queue_back(std::shared_ptr<queue_impl<T>> impl)
    : impl_(impl)
  {
  }

  bool pop(T& t)
  {
    std::unique_lock<std::mutex> lock(impl_->mutex_);
    while (impl_->queue_.empty() && !impl_->stop_)
      impl_->condition_.wait(lock);
    if (!impl_->queue_.empty())
    {
      t = impl_->queue_.front();
      impl_->queue_.pop();
      return true;
    }
    return false;
  }

private:
  std::shared_ptr<queue_impl<T>> impl_;
};

// Launch the last stage in a pipeline.
template <class T, class F>
std::future<void> pipeline(queue_back<T> in, F f)
{
  // Get the function's associated executor, defaulting to thread_executor.
  auto ex = get_associated_executor(f, new_thread_executor());

  // Run the function, and as we're the last stage return a future so that the
  // caller can wait for the pipeline to finish.
  return execution::require(ex,
      execution::single,
      execution::adaptable_blocking,
      execution::twoway
    ).twoway_execute(
      [in, f = std::move(f)]() mutable
      {
        f(in);
      });
}

// Launch an intermediate stage in a pipeline.
template <class T, class F, class... Tail>
std::future<void> pipeline(queue_back<T> in, F f, Tail... t)
{
  // Determine the output queue type.
  using output_value_type = typename std::tuple_element_t<1, args_tuple_t<F>>::value_type;

  // Create the output queue and its implementation.
  auto out_impl = std::make_shared<queue_impl<output_value_type>>();
  queue_front<output_value_type> out(out_impl);
  queue_back<output_value_type> next_in(out_impl);

  // Get the function's associated executor, defaulting to new_thread_executor.
  auto ex = get_associated_executor(f, new_thread_executor());

  // Run the function.
  execution::require(ex,
      execution::single,
      execution::oneway
    ).execute(
      [in, out, f = std::move(f)]() mutable
      {
        f(in, out);
        out.stop();
      });

  // Launch the rest of the pipeline.
  return pipeline(next_in, std::move(t)...);
}

// Launch the first stage in a pipeline.
template <class F, class... Tail>
std::future<void> pipeline(F f, Tail... t)
{
  // Determine the output queue type.
  using output_value_type = typename std::tuple_element_t<0, args_tuple_t<F>>::value_type;

  // Create the output queue and its implementation.
  auto out_impl = std::make_shared<queue_impl<output_value_type>>();
  queue_front<output_value_type> out(out_impl);
  queue_back<output_value_type> next_in(out_impl);

  // Get the function's associated executor, defaulting to new_thread_executor.
  auto ex = get_associated_executor(f, new_thread_executor());

  // Run the function.
  execution::require(ex,
      execution::single,
      execution::oneway
    ).execute(
      [out, f = std::move(f)]() mutable
      {
        f(out);
        out.stop();
      });

  // Launch the rest of the pipeline.
  return pipeline(next_in, std::move(t)...);
}

//------------------------------------------------------------------------------

#include <experimental/thread_pool>
#include <iostream>
#include <string>

void reader(queue_front<std::string> out)
{
  std::string line;
  while (std::getline(std::cin, line))
    out.push(line);
}

void filter(queue_back<std::string> in, queue_front<std::string> out)
{
  std::string line;
  while (in.pop(line))
    if (line.length() > 5)
      out.push(line);
}

void upper(queue_back<std::string> in, queue_front<std::string> out)
{
  std::string line;
  while (in.pop(line))
  {
    std::string new_line;
    for (char c : line)
      new_line.push_back(std::toupper(c));
    out.push(new_line);
  }
}

void writer(queue_back<std::string> in)
{
  std::size_t count = 0;
  std::string line;
  while (in.pop(line))
    std::cout << count++ << ": " << line << std::endl;
}

int main()
{
  std::experimental::static_thread_pool pool(1);

  auto f = pipeline(reader, filter, bind_executor(pool.executor(), upper), writer);
  f.wait();
}
