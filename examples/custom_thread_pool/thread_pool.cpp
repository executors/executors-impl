#include <chrono>
#include <execution>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

//------------------------------------------------------------------------------

struct nullary_function
{
  void operator()() {};
};

//------------------------------------------------------------------------------

struct preallocated_oneway_t;

template <class Executor, class = std::void_t<>>
struct is_preallocated_oneway_executor
  : std::false_type
{
};

template <class Executor>
struct is_preallocated_oneway_executor<Executor,
    std::enable_if_t<
      std::is_same_v<
        std::decay_t<decltype(std::execution::executor_concept_t::static_query_v<Executor>)>,
        preallocated_oneway_t
      >
    >
  > : std::true_type
{
};

struct preallocated_oneway_t
{
  static constexpr bool is_requirable_concept = true;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  template<class Executor>
    static constexpr bool static_query_v
      = is_preallocated_oneway_executor<Executor>::value;

  static constexpr bool value() { return true; }
};

inline constexpr preallocated_oneway_t preallocated_oneway;

namespace std
{
  template<class Entity>
  struct is_applicable_property<Entity, preallocated_oneway_t,
    std::enable_if_t<std::execution::is_executor<Entity>::value>>
      : std::true_type {};
}

//------------------------------------------------------------------------------

template <class Function>
struct allocated_size_of_t
{
  static constexpr bool is_requirable_concept = false;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = std::size_t;
};

template <class Function>
inline constexpr allocated_size_of_t<Function> allocated_size_of;

namespace std
{
  template<class Entity, class Function>
  struct is_applicable_property<Entity, ::allocated_size_of_t<Function>,
    std::enable_if_t<is_preallocated_oneway_executor<Entity>::value>>
      : std::true_type {};
}

//------------------------------------------------------------------------------

template <class Function>
struct alignment_of_t
{
  static constexpr bool is_requirable_concept = false;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = std::size_t;
};

template <class Function>
inline constexpr alignment_of_t<Function> alignment_of;

namespace std
{
  template<class Entity, class Function>
  struct is_applicable_property<Entity, ::alignment_of_t<Function>,
    std::enable_if_t<is_preallocated_oneway_executor<Entity>::value>>
      : std::true_type {};
}

//------------------------------------------------------------------------------

struct intrusive_oneway_t;

template <class Executor, class = std::void_t<>>
struct is_intrusive_oneway_executor
  : std::false_type
{
};

template <class Executor>
struct is_intrusive_oneway_executor<Executor,
    std::enable_if_t<
      std::is_same_v<
        std::decay_t<decltype(std::execution::executor_concept_t::static_query_v<Executor>)>,
        intrusive_oneway_t
      >
    >
  > : std::true_type
{
};

struct intrusive_oneway_t
{
  static constexpr bool is_requirable_concept = true;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  template<class Executor>
    static constexpr bool static_query_v
      = is_intrusive_oneway_executor<Executor>::value;

  static constexpr bool value() { return true; }
};

inline constexpr intrusive_oneway_t intrusive_oneway;

namespace std
{
  template<class Entity>
  struct is_applicable_property<Entity, intrusive_oneway_t,
    std::enable_if_t<std::execution::is_executor<Entity>::value>>
      : std::true_type {};
}

//------------------------------------------------------------------------------

class thread_pool
{
public:
  explicit thread_pool(std::size_t n)
  {
    for (std::size_t i = 0; i < n; ++i)
      threads_.emplace_back([this]{ run(); });
  }

  ~thread_pool()
  {
    wait();
  }

  void wait()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    std::list<std::thread> threads(std::move(threads_));
    if (!threads.empty())
    {
      stopped_ = true;
      condition_.notify_all();
      lock.unlock();
      for (auto& t : threads)
        t.join();
    }
  }

  class preallocated_oneway_executor;
  class intrusive_oneway_executor;

  template <class ProtoAllocator = std::allocator<void>>
  class oneway_executor
  {
  public:
    oneway_executor(const oneway_executor& other) noexcept = default;

    static constexpr auto query(std::execution::executor_concept_t) { return std::execution::oneway; }

    template <class Function>
    void execute(Function f) const
    {
      pool_.add(function_with_allocator<Function, ProtoAllocator>::create(std::move(f), alloc_));
    }

    bool operator==(const oneway_executor& other) const noexcept
    {
      return &pool_ == &other.pool_;
    }

    bool operator!=(const oneway_executor& other) const noexcept
    {
      return &pool_ != &other.pool_;
    }

    preallocated_oneway_executor require_concept(const preallocated_oneway_t&) const
    {
      return {pool_};
    }

    intrusive_oneway_executor require_concept(const intrusive_oneway_t&) const
    {
      return {pool_};
    }

    oneway_executor<std::allocator<void>> require(const std::execution::allocator_t<void>&) const
    {
      return {pool_, std::allocator<void>{}};
    }

    template<class NewProtoAllocator>
    oneway_executor<NewProtoAllocator> require(const std::execution::allocator_t<NewProtoAllocator>& a) const
    {
      return {pool_, a.value()};
    }

    ProtoAllocator query(const std::execution::allocator_t<ProtoAllocator>&) const noexcept
    {
      return alloc_;
    }

    ProtoAllocator query(const std::execution::allocator_t<void>&) const noexcept
    {
      return alloc_;
    }

  private:
    friend class thread_pool;

    oneway_executor(thread_pool& owner, const ProtoAllocator& a = std::allocator<void>())
      : pool_(owner),
        alloc_(a)
    {
    }

    thread_pool& pool_;
    ProtoAllocator alloc_;
  };

  class preallocated_oneway_executor
  {
  public:
    preallocated_oneway_executor(const preallocated_oneway_executor& other) noexcept = default;

    static constexpr auto query(std::execution::executor_concept_t) { return preallocated_oneway; }

    template <class Function>
    void execute(Function f, void* mem, std::size_t size) const
    {
      pool_.add(function_with_preallocated_memory<Function>::create(std::move(f), mem, size));
    }

    bool operator==(const preallocated_oneway_executor& other) const noexcept
    {
      return &pool_ == &other.pool_;
    }

    bool operator!=(const preallocated_oneway_executor& other) const noexcept
    {
      return &pool_ != &other.pool_;
    }

    oneway_executor<> require_concept(const std::execution::oneway_t&) const
    {
      return {pool_};
    }

    intrusive_oneway_executor require_concept(const intrusive_oneway_t&) const
    {
      return {pool_};
    }

    template <class Function>
    static constexpr std::size_t query(const allocated_size_of_t<Function>&) noexcept
    {
      return sizeof(function_with_preallocated_memory<Function>);
    }

    template <class Function>
    static constexpr std::size_t query(const alignment_of_t<Function>&) noexcept
    {
      return alignof(function_with_preallocated_memory<Function>);
    }

  private:
    friend class thread_pool;

    preallocated_oneway_executor(thread_pool& owner)
      : pool_(owner)
    {
    }

    thread_pool& pool_;
  };

private:
  template <class Function> struct function_with_intrusive_memory;

public:
  class intrusive_oneway_executor
  {
  public:
    intrusive_oneway_executor(const intrusive_oneway_executor& other) noexcept = default;

    static constexpr auto query(std::execution::executor_concept_t) { return intrusive_oneway; }

    template <class Function>
    function_with_intrusive_memory<Function> package(Function f) const
    {
      return function_with_intrusive_memory<Function>(std::move(f));
    }

    template <class Function>
    void submit(function_with_intrusive_memory<Function>& f) const
    {
      pool_.add(&f);
    }

    bool operator==(const intrusive_oneway_executor& other) const noexcept
    {
      return &pool_ == &other.pool_;
    }

    bool operator!=(const intrusive_oneway_executor& other) const noexcept
    {
      return &pool_ != &other.pool_;
    }

    oneway_executor<> require_concept(const std::execution::oneway_t&) const
    {
      return {pool_};
    }

    preallocated_oneway_executor require_concept(const preallocated_oneway_t&) const
    {
      return {pool_};
    }

  private:
    friend class thread_pool;

    intrusive_oneway_executor(thread_pool& owner)
      : pool_(owner)
    {
    }

    thread_pool& pool_;
  };

  using executor_type = oneway_executor<>;

  executor_type executor() noexcept
  {
    return executor_type(*this);
  }

private:
  void run()
  {
    for (std::unique_lock<std::mutex> lock(mutex_);; lock.lock())
    {
      condition_.wait(lock, [this]{ return stopped_ || head_; });
      if (!head_) return;
      function* func = head_.release();
      head_ = std::move(func->next_);
      tail_ = head_ ? tail_ : &head_;
      lock.unlock();
      func->call(nullptr);
    }
  }

  template <class Function>
  struct deleter
  {
    void operator()(Function* p)
    {
      p->destroy();
    }
  };

  struct function
  {
    virtual ~function() {}
    virtual void call(void*) = 0;
    virtual void destroy() = 0;

    std::unique_ptr<function, deleter<function>> next_;
  };

  void add(function* f)
  {
    std::unique_ptr<function, deleter<function>> p(f);
    std::unique_lock<std::mutex> lock(mutex_);
    *tail_ = std::move(p);
    tail_ = &(*tail_)->next_;
    condition_.notify_one();
  }

  template <class Function, class ProtoAllocator>
  struct function_with_allocator : function
  {
    explicit function_with_allocator(Function f, const ProtoAllocator& a)
      : func_(std::move(f)),
        alloc_(a)
    {
    }

    static function* create(Function f, const ProtoAllocator& a)
		{
			typename std::allocator_traits<ProtoAllocator>::template rebind_alloc<function_with_allocator> allocator(a);
			function_with_allocator* raw_p = allocator.allocate(1);
			try
			{
        return new (raw_p) function_with_allocator(std::move(f), a);
			}
			catch (...)
			{
				allocator.deallocate(raw_p, 1);
				throw;
			}
		}

    void call(void*) final override
    {
      std::unique_ptr<function_with_allocator, deleter<function_with_allocator>> p(this);
      Function f(std::move(func_));
      p.reset();
      [&f]() noexcept { f(); }();
    }

    void destroy() final override
    {
      function_with_allocator* p = this;
      p->~function_with_allocator();
      alloc_.deallocate(p, 1);
    }

    Function func_;
    typename std::allocator_traits<ProtoAllocator>::template rebind_alloc<function_with_allocator> alloc_;
  };

  template <class Function>
  struct function_with_preallocated_memory : function
  {
    explicit function_with_preallocated_memory(Function f)
      : func_(std::move(f))
    {
    }

    static function* create(Function f, void* mem, std::size_t size)
		{
      assert(size >= sizeof(function_with_preallocated_memory));
      function_with_preallocated_memory* raw_p = static_cast<function_with_preallocated_memory*>(mem);
      return new (raw_p) function_with_preallocated_memory(std::move(f));
		}

    void call(void*) final override
    {
      std::unique_ptr<function_with_preallocated_memory, deleter<function_with_preallocated_memory>> p(this);
      Function f(std::move(func_));
      p.reset();
      [&f]() noexcept { f(); }();
    }

    void destroy() final override
    {
      function_with_preallocated_memory* p = this;
      p->~function_with_preallocated_memory();
    }

    Function func_;
  };

  template <class Function>
  struct function_with_intrusive_memory : function
  {
    explicit function_with_intrusive_memory(Function f)
      : func_(std::move(f))
    {
    }

    void call(void*) final override
    {
      Function f(std::move(func_));
      [&f]() noexcept { f(); }();
    }

    void destroy() final override
    {
    }

    Function func_;
  };

  std::mutex mutex_;
  std::condition_variable condition_;
  std::list<std::thread> threads_;
  std::unique_ptr<function, deleter<function>> head_;
  std::unique_ptr<function, deleter<function>>* tail_{&head_};
  bool stopped_ = false;
};

static_assert(std::execution::is_oneway_executor_v<thread_pool::executor_type>);
static_assert(!std::execution::is_oneway_executor_v<thread_pool::preallocated_oneway_executor>);
static_assert(!std::execution::is_oneway_executor_v<thread_pool::intrusive_oneway_executor>);
static_assert(!is_preallocated_oneway_executor<thread_pool::executor_type>::value);
static_assert(is_preallocated_oneway_executor<thread_pool::preallocated_oneway_executor>::value);
static_assert(!is_preallocated_oneway_executor<thread_pool::intrusive_oneway_executor>::value);
static_assert(!is_intrusive_oneway_executor<thread_pool::executor_type>::value);
static_assert(!is_intrusive_oneway_executor<thread_pool::preallocated_oneway_executor>::value);
static_assert(is_intrusive_oneway_executor<thread_pool::intrusive_oneway_executor>::value);

//------------------------------------------------------------------------------

int main()
{
  alignas(std::max_align_t) unsigned char mem1[1024];
  alignas(std::max_align_t) unsigned char mem2[1024];
  thread_pool tp(4);

  auto ex1 = tp.executor();
  ex1.execute(
    []{
      std::cout << "1: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "1: after sleep\n";
    });

  auto ex2 = std::require(ex1, std::execution::allocator(std::allocator<int>()));
  ex2.execute(
    []{
      std::cout << "2: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "2: after sleep\n";
    });

  auto ex3 = std::require(ex2, std::execution::allocator);
  ex3.execute(
    []{
      std::cout << "3: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "3: after sleep\n";
    });

  auto ex4 = std::require_concept(ex3, preallocated_oneway);
  ex4.execute(
    []{
      std::cout << "4: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "4: after sleep\n";
    }, mem1, sizeof(mem1));

  auto ex5 = std::require_concept(ex4, preallocated_oneway);
  ex5.execute(
    []{
      std::cout << "5: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "5: after sleep\n";
    }, mem2, sizeof(mem2));

  auto ex6 = std::require_concept(ex5, std::execution::oneway);
  ex6.execute(
    []{
      std::cout << "6: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "6: after sleep\n";
    });

  auto ex7 = std::require_concept(ex6, std::execution::oneway);
  ex7.execute(
    []{
      std::cout << "7: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "7: after sleep\n";
    });

  struct my_function
  {
    char big[4096];

    void operator()()
    {
      std::cout << "8: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "8: after sleep\n";
    }
  };

  auto ex8 = std::require_concept(ex7, preallocated_oneway);
  constexpr std::size_t preallocated_size = std::query(ex8, allocated_size_of<my_function>);
  constexpr std::size_t preallocated_align = std::query(ex8, alignment_of<my_function>);
  alignas(preallocated_align) static unsigned char mem3[preallocated_size];
  ex8.execute(my_function{}, mem3, preallocated_size);

  auto ex9 = std::require_concept(tp.executor(), intrusive_oneway);
  auto package = ex9.package(
    []{
      std::cout << "9: before sleep\n";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "9: after sleep\n";
    });
  ex9.submit(package);

  tp.wait();
}
