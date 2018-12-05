#ifndef STD_BITS_EXECUTION_PREFER_H_INCLUDED
#define STD_BITS_EXECUTION_PREFER_H_INCLUDED

#include <bits/execution/prefer_free_traits.h>
#include <bits/execution/require_member_traits.h>
#include <bits/execution/require_static_traits.h>
#include <utility>

namespace std {
namespace execution {
namespace prefer_impl {

struct prefer_fn
{
  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&&) const
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && impl::require_static_traits<Executor, Property>::is_valid,
      typename impl::require_static_traits<Executor, Property>::result_type
    >
  {
    return std::forward<Executor>(ex);
  }

  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&& p) const
    noexcept(impl::require_member_traits<Executor, Property>::is_noexcept)
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && !impl::require_static_traits<Executor, Property>::is_valid
        && impl::require_member_traits<Executor, Property>::is_valid,
      typename impl::require_member_traits<Executor, Property>::result_type
    >
  {
    return std::forward<Executor>(ex).require(std::forward<Property>(p));
  }

  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&& p) const
    noexcept(impl::prefer_free_traits<Executor, Property>::is_noexcept)
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && !impl::require_static_traits<Executor, Property>::is_valid
        && !impl::require_member_traits<Executor, Property>::is_valid
        && impl::prefer_free_traits<Executor, Property>::is_valid,
      typename impl::prefer_free_traits<Executor, Property>::result_type
    >
  {
    return prefer(std::forward<Executor>(ex), std::forward<Property>(p));
  }

  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&&) const
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && !impl::require_static_traits<Executor, Property>::is_valid
        && !impl::require_member_traits<Executor, Property>::is_valid
        && !impl::prefer_free_traits<Executor, Property>::is_valid,
      typename std::decay_t<Executor>
    >
  {
    return std::forward<Executor>(ex);
  }

  template<class Executor, class Property0, class Property1, class... PropertyN>
  constexpr auto operator()(Executor&& ex, Property0&& p0, Property1&& p1, PropertyN&&... pn) const
    noexcept(noexcept(std::declval<prefer_fn>()(std::declval<prefer_fn>()(std::forward<Executor>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...)))
    -> decltype(std::declval<prefer_fn>()(std::declval<prefer_fn>()(std::forward<Executor>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...))
  {
    return (*this)((*this)(std::forward<Executor>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...);
  }
};

template<class T = prefer_fn> constexpr T customization_point{};

} // namespace prefer_impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_PREFER_H_INCLUDED
