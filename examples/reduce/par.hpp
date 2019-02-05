#pragma once

#include <experimental/execution>
#include <experimental/thread_pool>
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
  std::experimental::execution::can_require_v<
    Executor
    , ExecutionRequirement
    , std::experimental::execution::bulk_t
    , std::experimental::execution::oneway_t
    , std::experimental::execution::blocking_t::always_t
    , std::experimental::execution::mapping_t::thread_t
>;


} // end impl


class parallel_policy
{
  public:
    static constexpr std::experimental::execution::bulk_guarantee_t::parallel_t execution_requirement{};

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


std::experimental::static_thread_pool system_thread_pool{std::max(1u,std::thread::hardware_concurrency())};


} // end impl

