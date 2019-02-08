#pragma once

#include <execution>
#include <thread_pool>
#include <thread>
#include <algorithm>


namespace impl
{


template<class, class> class basic_execution_policy;


template<class ER, class E>
basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex);


template<class ExecutionRequirement, class Executor>
class basic_execution_policy
{
  public:
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


template<class Executor, class ExecutionRequirement>
constexpr bool satisfies_cpp20_on_requirements_v =
  std::can_require_concept_v<
    Executor
    , std::execution::bulk_oneway_t
> &&
  std::can_require_v<
    Executor
    , ExecutionRequirement
    , std::execution::blocking_t::always_t
    , std::execution::mapping_t::thread_t
>;


} // end impl


class parallel_policy
{
  public:
    static constexpr std::execution::bulk_guarantee_t::parallel_t execution_requirement{};

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


namespace impl
{


std::static_thread_pool system_thread_pool{std::max(1u,std::thread::hardware_concurrency())};


} // end impl


template<class Iterator, class Size, class Function>
void for_each_n(const parallel_policy&, Iterator first, Size n, Function f)
{
  return for_each_n(par.on(impl::system_thread_pool.executor()), first, n, f);
}

