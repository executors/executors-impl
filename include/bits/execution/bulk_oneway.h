#ifndef STD_BITS_EXECUTION_BULK_ONEWAY_H_INCLUDED
#define STD_BITS_EXECUTION_BULK_ONEWAY_H_INCLUDED

#include <bits/execution/adapter.h>
#include <bits/execution/is_bulk_oneway_executor.h>
#include <bits/execution/is_executor.h>
#include <bits/execution/is_oneway_executor.h>
#include <memory>

namespace std {
namespace execution {

struct bulk_oneway_t
{
  static constexpr bool is_requirable_concept = true;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  template<class... SupportableProperties>
    class polymorphic_executor_type;

  template<class Executor>
    static constexpr bool static_query_v
      = is_bulk_oneway_executor<Executor>::value;

  static constexpr bool value() { return true; }

private:
  template<class Executor>
  class adapter : public impl::adapter<adapter, Executor>
  {
  public:
    using impl::adapter<adapter, Executor>::adapter;
    using impl::adapter<adapter, Executor>::require;

    static constexpr bulk_oneway_t query(executor_concept_t) { return {}; }

    Executor require_concept(oneway_t) const noexcept
    {
      return this->executor_;
    }

    template<class Function, class SharedFactory>
    void bulk_execute(Function f, std::size_t n, SharedFactory sf) const
    {
      auto shared_state = std::make_shared<decltype(sf())>(sf());
      for (std::size_t i = 0; i < n; ++i)
      {
        this->executor_.execute(
            [f = std::move(f), i, shared_state]() mutable
            {
              f(i, *shared_state);
            });
      }
    }
  };

public:
  // Default require for bulk adapts single executors.
  template <typename Executor, typename =
      std::enable_if_t<
        is_oneway_executor<Executor>::value && !is_bulk_oneway_executor<Executor>::value,
        adapter<Executor>
      >>
  friend adapter<Executor> require_concept(Executor ex, bulk_oneway_t)
  {
    return adapter<Executor>(std::move(ex));
  }
};

constexpr bulk_oneway_t bulk_oneway;

} // namespace execution

template<class Entity>
struct is_applicable_property<Entity, execution::bulk_oneway_t,
  std::enable_if_t<execution::is_executor_v<Entity>>> : std::true_type {};

} // namespace std

#endif // STD_BITS_EXECUTION_BULK_ONEWAY_H_INCLUDED
