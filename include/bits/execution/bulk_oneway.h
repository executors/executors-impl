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

#if defined(__cpp_concepts)
  template<class Executor>
    static constexpr bool static_query_v
      = BulkOneWayExecutor<Executor>;
#else
  template<class Executor>
    static constexpr bool static_query_v
      = is_bulk_oneway_executor<Executor>::value;
#endif

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
      auto shared_index = std::make_shared<std::atomic<std::size_t>>(0);
      auto shared_state = std::make_shared<decltype(sf())>(sf());
      for (std::size_t i = 0; i < n; ++i)
      {
        try
        {
          this->executor_.execute(
              [f = std::move(f), shared_index, n, shared_state]() mutable
              {
                for (std::size_t index = shared_index->load(); index < n;)
                  if (shared_index->compare_exchange_weak(index, index + 1))
                    f(index, *shared_state);
              });
        }
        catch (...)
        {
          if (i == 0)
            throw;
          else
            break;
        }
      }
    }
  };

public:
  // Default require for bulk adapts single executors.
#if defined(__cpp_concepts)
  template<OneWayExecutor E>
  friend adapter<E> require_concept(E ex, bulk_oneway_t)
  {
    return adapter<E>(std::move(ex));
  }
#else
  template <typename Executor, typename =
      std::enable_if_t<
        is_oneway_executor<Executor>::value && !is_bulk_oneway_executor<Executor>::value,
        adapter<Executor>
      >>
  friend adapter<Executor> require_concept(Executor ex, bulk_oneway_t)
  {
    return adapter<Executor>(std::move(ex));
  }
#endif
};

constexpr bulk_oneway_t bulk_oneway;

} // namespace execution

#if defined(__cpp_concepts)
template<execution::Executor E>
struct is_applicable_property<E, execution::bulk_oneway_t> : std::true_type {};
#else
template<class Entity>
struct is_applicable_property<Entity, execution::bulk_oneway_t,
  std::enable_if_t<execution::is_executor_v<Entity>>> : std::true_type {};
#endif

} // namespace std

#endif // STD_BITS_EXECUTION_BULK_ONEWAY_H_INCLUDED
