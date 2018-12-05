#ifndef STD_BITS_PROPERTIES_PREFER_H_INCLUDED
#define STD_BITS_PROPERTIES_PREFER_H_INCLUDED

#include <bits/properties/prefer_free_traits.h>
#include <bits/properties/require_member_traits.h>
#include <bits/properties/require_static_traits.h>
#include <utility>

namespace std {
namespace prefer_impl {

struct prefer_fn
{
  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&&) const
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && impl::require_static_traits<Entity, Property>::is_valid,
      typename impl::require_static_traits<Entity, Property>::result_type
    >
  {
    return std::forward<Entity>(ex);
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&& p) const
    noexcept(impl::require_member_traits<Entity, Property>::is_noexcept)
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && !impl::require_static_traits<Entity, Property>::is_valid
        && impl::require_member_traits<Entity, Property>::is_valid,
      typename impl::require_member_traits<Entity, Property>::result_type
    >
  {
    return std::forward<Entity>(ex).require(std::forward<Property>(p));
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&& p) const
    noexcept(impl::prefer_free_traits<Entity, Property>::is_noexcept)
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && !impl::require_static_traits<Entity, Property>::is_valid
        && !impl::require_member_traits<Entity, Property>::is_valid
        && impl::prefer_free_traits<Entity, Property>::is_valid,
      typename impl::prefer_free_traits<Entity, Property>::result_type
    >
  {
    return prefer(std::forward<Entity>(ex), std::forward<Property>(p));
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&&) const
    -> std::enable_if_t<
      std::decay_t<Property>::is_preferable
        && !impl::require_static_traits<Entity, Property>::is_valid
        && !impl::require_member_traits<Entity, Property>::is_valid
        && !impl::prefer_free_traits<Entity, Property>::is_valid,
      typename std::decay_t<Entity>
    >
  {
    return std::forward<Entity>(ex);
  }

  template<class Entity, class Property0, class Property1, class... PropertyN>
  constexpr auto operator()(Entity&& ex, Property0&& p0, Property1&& p1, PropertyN&&... pn) const
    noexcept(noexcept(std::declval<prefer_fn>()(std::declval<prefer_fn>()(std::forward<Entity>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...)))
    -> decltype(std::declval<prefer_fn>()(std::declval<prefer_fn>()(std::forward<Entity>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...))
  {
    return (*this)((*this)(std::forward<Entity>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...);
  }
};

template<class T = prefer_fn> constexpr T customization_point{};

} // namespace prefer_impl
} // namespace std

#endif // STD_BITS_PROPERTIES_PREFER_H_INCLUDED
