#ifndef STD_EXPERIMENTAL_BITS_BULK_ONEWAY_H
#define STD_EXPERIMENTAL_BITS_BULK_ONEWAY_H

#include <experimental/bits/adapter.h>
#include <experimental/bits/is_bulk_oneway_executor.h>
#include <experimental/bits/is_oneway_executor.h>
#include <memory>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct bulk_oneway_t
{
  static constexpr bool is_requirable = true;
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

    Executor require(oneway_t) const noexcept
    {
      return this->executor_;
    }

    template<class Function, class SharedFactory>
    void execute(Function f, std::size_t n, SharedFactory sf) const
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
  friend adapter<Executor> require(Executor ex, bulk_oneway_t)
  {
    return adapter<Executor>(std::move(ex));
  }
};

constexpr bulk_oneway_t bulk_oneway;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_BULK_ONEWAY_H
