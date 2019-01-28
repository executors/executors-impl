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


int main()
{
  std::vector<int> vec(10);

  // test with static_thread_pool::executor_type
  std::experimental::static_thread_pool pool(4);

  for_each_n(par.on(pool.executor()), vec.begin(), vec.size(), [](int& x)
  {
    x = 42;
  });

  assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));

  std::cout << "OK" << std::endl;

  return 0;
}

