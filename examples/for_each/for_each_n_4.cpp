#include <experimental/thread_pool>
#include <experimental/execution>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>


namespace execution = std::experimental::execution;
using std::experimental::static_thread_pool;


namespace impl
{


template<class, class> class basic_execution_policy;


template<class ER, class E>
basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex);


template<class ExecutionRequirement, class Executor>
class basic_execution_policy
{
  public:
    static_assert(execution::can_require_v<Executor, execution::bulk_t, ExecutionRequirement>);

    static constexpr ExecutionRequirement execution_requirement{};

    Executor executor() const
    {
      return executor_;
    }

  private:
    template<class ER, class E>
    friend basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex);

    basic_execution_policy(const Executor& executor)
      : executor_(executor)
    {}

    Executor executor_;
};


template<class ER, class E>
basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex)
{
  return basic_execution_policy<ER,E>{ex};
}


constexpr struct ignored {} ignore;


template<class Executor, class ExecutionRequirement>
constexpr bool satisfies_cpp20_on_requirements_v =
  execution::can_require_v<
    Executor
    , ExecutionRequirement
    , execution::bulk_t
    , execution::oneway_t
    , execution::blocking_t::always_t
    , execution::mapping_t::thread_t
>;


} // end impl


class parallel_policy
{
  public:
    static constexpr execution::bulk_guarantee_t::parallel_t execution_requirement{};

    template<class Executor,
             class = std::enable_if_t<
               impl::satisfies_cpp20_on_requirements_v<Executor, decltype(execution_requirement)>
             >>
    impl::basic_execution_policy<decltype(execution_requirement), Executor> on(const Executor& ex) const
    {
      return impl::make_basic_execution_policy<decltype(execution_requirement)>(ex);
    }
};


constexpr parallel_policy par{};


template<class RandomAccessIterator, class Size, class Function>
void for_each_n(const parallel_policy&, RandomAccessIterator first, Size n, Function f)
{
  throw std::runtime_error("for_each_n(parallel_policy): Unimplemented.");
}


template<class ExecutionPolicy, class RandomAccessIterator, class Size, class Function>
void for_each_n(ExecutionPolicy&& policy, RandomAccessIterator first, Size n, Function f)
{
  auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always, execution::mapping.thread);

  try
  {
    // this only throws upon failure to create EAs
    ex.bulk_execute(
      [=](size_t idx, impl::ignored&)
      {
        // XXX terminate if this throws?
        f(first[idx]);
      },
      n,
      []{ return impl::ignore; }
    );
  }
  catch(...)
  {
    // sequential fallback
    for(Size i = 0; i < n; ++i, ++first)
    {
      f(*first);
    }
  }
}


class bulk_inline_executor
{
  public:
    friend bool operator==(const bulk_inline_executor&, const bulk_inline_executor&) noexcept
    {
      return true;
    }

    friend bool operator!=(const bulk_inline_executor&, const bulk_inline_executor&) noexcept
    {
      return false;
    }

    bulk_inline_executor require(execution::oneway_t) const
    {
      return *this;
    }

    bulk_inline_executor require(execution::bulk_t) const
    {
      return *this;
    }

    constexpr static execution::bulk_guarantee_t::parallel_t query(execution::bulk_guarantee_t)
    {
      return execution::bulk_guarantee.parallel;
    }

    constexpr static execution::blocking_t::always_t query(execution::blocking_t)
    {
      return execution::blocking.always;
    }

    constexpr static execution::mapping_t::thread_t query(execution::mapping_t)
    {
      return execution::mapping.thread;
    }

    template<class Function, class Factory>
    void bulk_execute(Function f, size_t n, Factory shared_factory) const noexcept
    {
      try
      {
        auto shared = shared_factory();

        for(size_t i = 0; i < n; ++i)
        {
          f(i, shared);
        }
      }
      catch(...)
      {
      }
    }
};


int main()
{
  std::vector<int> vec(10);

  // test with bulk_inline_executor

  for_each_n(par.on(bulk_inline_executor()), vec.begin(), vec.size(), [](int& x)
  {
    x = 42;
  });

  assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));

  std::cout << "OK" << std::endl;

  return 0;
}

