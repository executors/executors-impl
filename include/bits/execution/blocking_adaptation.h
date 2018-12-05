#ifndef STD_BITS_EXECUTION_BLOCKING_ADAPTATION_H_INCLUDED
#define STD_BITS_EXECUTION_BLOCKING_ADAPTATION_H_INCLUDED

#include <bits/execution/enumeration.h>
#include <bits/execution/enumerator_adapter.h>

namespace std {
namespace execution {

struct blocking_adaptation_t :
  impl::enumeration<blocking_adaptation_t, 2>
{
  using impl::enumeration<blocking_adaptation_t, 2>::enumeration;

  using disallowed_t = enumerator<0>;
  using allowed_t = enumerator<1>;

  static constexpr disallowed_t disallowed{};
  static constexpr allowed_t allowed{};

private:
  template <typename Executor>
  class adapter :
    public impl::enumerator_adapter<adapter,
      Executor, blocking_adaptation_t, allowed_t>
  {
    template <class T> static auto inner_declval() -> decltype(std::declval<Executor>());

  public:
    using impl::enumerator_adapter<adapter, Executor,
      blocking_adaptation_t, allowed_t>::enumerator_adapter;

    template<class Function> auto execute(Function f) const
      -> decltype(inner_declval<Function>().execute(std::move(f)))
    {
      return this->executor_.execute(std::move(f));
    }

    template<class Function>
    auto twoway_execute(Function f) const
      -> decltype(inner_declval<Function>().twoway_execute(std::move(f)))
    {
      return this->executor_.twoway_execute(std::move(f));
    }

    template<class Function, class SharedFactory>
    auto bulk_execute(Function f, std::size_t n, SharedFactory sf) const
      -> decltype(inner_declval<Function>().bulk_execute(std::move(f), n, std::move(sf)))
    {
      return this->executor_.bulk_execute(std::move(f), n, std::move(sf));
    }

    template<class Function, class ResultFactory, class SharedFactory>
    auto bulk_twoway_execute(Function f, std::size_t n, ResultFactory rf, SharedFactory sf) const
      -> decltype(inner_declval<Function>().bulk_twoway_execute(std::move(f), n, std::move(rf), std::move(sf)))
    {
      return this->executor_.bulk_twoway_execute(std::move(f), n, std::move(rf), std::move(sf));
    }
  };

public:
  template <typename Executor>
  friend adapter<Executor> require(Executor ex, allowed_t)
  {
    return adapter<Executor>(std::move(ex));
  }
};

constexpr blocking_adaptation_t blocking_adaptation{};
inline constexpr blocking_adaptation_t::disallowed_t blocking_adaptation_t::disallowed;
inline constexpr blocking_adaptation_t::allowed_t blocking_adaptation_t::allowed;

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_BLOCKING_ADAPTATION_H_INCLUDED
