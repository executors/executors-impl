#ifndef STD_EXPERIMENTAL_BITS_PREFER_H
#define STD_EXPERIMENTAL_BITS_PREFER_H

#include <experimental/bits/prefer_free_traits.h>
#include <experimental/bits/require_member_traits.h>
#include <experimental/bits/require_static_traits.h>
#include <utility>

namespace std {
namespace experimental {
inline namespace executors_v1 {
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
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_PREFER_H
