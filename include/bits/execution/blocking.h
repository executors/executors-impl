#ifndef STD_BITS_EXECUTION_BLOCKING_H_INCLUDED
#define STD_BITS_EXECUTION_BLOCKING_H_INCLUDED

#include <bits/execution/blocking_adaptation.h>
#include <bits/execution/enumeration.h>
#include <bits/execution/enumerator_adapter.h>
#include <future>

namespace std {
namespace execution {

struct blocking_t :
  impl::enumeration<blocking_t, 3>
{
  using impl::enumeration<blocking_t, 3>::enumeration;

  using possibly_t = enumerator<0>;
  using always_t = enumerator<1>;
  using never_t = enumerator<2>;

  static constexpr possibly_t possibly{};
  static constexpr always_t always{};
  static constexpr never_t never{};

private:
  template<class Executor>
  class adapter :
    public impl::enumerator_adapter<adapter, Executor, blocking_t, always_t>
  {
    template <class T> static auto inner_declval() -> decltype(std::declval<Executor>());

  public:
    using impl::enumerator_adapter<adapter, Executor, blocking_t, always_t>::enumerator_adapter;

    template<class Function> auto execute(Function f) const
      -> decltype(inner_declval<Function>().execute(std::move(f)))
    {
      std::promise<void> promise;
      std::future<void> future = promise.get_future();
      this->executor_.execute([f = std::move(f), p = std::move(promise)]() mutable { f(); });
      future.wait();
    }

    template<class Function>
    auto twoway_execute(Function f) const
      -> decltype(inner_declval<Function>().twoway_execute(std::move(f)))
    {
      auto future = this->executor_.twoway_execute(std::move(f));
      future.wait();
      return future;
    }

    template<class Function, class SharedFactory>
    auto bulk_execute(Function f, std::size_t n, SharedFactory sf) const
      -> decltype(inner_declval<Function>().bulk_execute(std::move(f), n, std::move(sf)))
    {
      std::promise<void> promise;
      std::future<void> future = promise.get_future();
      this->executor_.bulk_execute([f = std::move(f), p = std::move(promise)](auto i, auto& s) mutable { f(i, s); }, n, std::move(sf));
      future.wait();
    }

    template<class Function, class ResultFactory, class SharedFactory>
    auto bulk_twoway_execute(Function f, std::size_t n, ResultFactory rf, SharedFactory sf) const
      -> decltype(inner_declval<Function>().bulk_twoway_execute(std::move(f), n, std::move(rf), std::move(sf)))
    {
      auto future = this->executor_.bulk_twoway_execute(std::move(f), n, std::move(rf), std::move(sf));
      future.wait();
      return future;
    }
  };

public:
  template<class Executor,
    typename = std::enable_if_t<
      blocking_adaptation_t::static_query_v<Executor>
        == blocking_adaptation.allowed
    >>
  friend adapter<Executor> require(Executor ex, always_t)
  {
    return adapter<Executor>(std::move(ex));
  }
};

constexpr blocking_t blocking{};
inline constexpr blocking_t::possibly_t blocking_t::possibly;
inline constexpr blocking_t::always_t blocking_t::always;
inline constexpr blocking_t::never_t blocking_t::never;

} // namespace execution

template<class Entity>
struct is_applicable_property<Entity, execution::blocking_t,
  std::enable_if_t<execution::is_oneway_executor_v<Entity> || execution::is_bulk_oneway_executor_v<Entity>>>
    : std::true_type {};

} // namespace std

#endif // STD_BITS_EXECUTION_BLOCKING_H_INCLUDED
