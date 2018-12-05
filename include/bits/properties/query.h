#ifndef STD_BITS_PROPERTIES_QUERY_H_INCLUDED
#define STD_BITS_PROPERTIES_QUERY_H_INCLUDED

#include <bits/properties/query_free_traits.h>
#include <bits/properties/query_member_traits.h>
#include <bits/properties/query_static_traits.h>
#include <utility>

namespace std {
namespace query_impl {

struct query_fn
{
  template<class Entity, class Property>
  constexpr auto operator()(Entity&&, Property&&) const
    -> std::enable_if_t<
      impl::query_static_traits<Entity, Property>::is_valid,
      typename impl::query_static_traits<Entity, Property>::result_type
    >
  {
    return std::decay_t<Property>::template static_query_v<std::decay_t<Entity>>;
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&& p) const
    noexcept(impl::query_member_traits<Entity, Property>::is_noexcept)
    -> std::enable_if_t<
      !impl::query_static_traits<Entity, Property>::is_valid
        && impl::query_member_traits<Entity, Property>::is_valid,
      typename impl::query_member_traits<Entity, Property>::result_type
    >
  {
    return std::forward<Entity>(ex).query(std::forward<Property>(p));
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&& p) const
    noexcept(impl::query_free_traits<Entity, Property>::is_noexcept)
    -> std::enable_if_t<
      !impl::query_static_traits<Entity, Property>::is_valid
        && !impl::query_member_traits<Entity, Property>::is_valid
        && impl::query_free_traits<Entity, Property>::is_valid,
      typename impl::query_free_traits<Entity, Property>::result_type
    >
  {
    return query(std::forward<Entity>(ex), std::forward<Property>(p));
  }
};

template<class T = query_fn> constexpr T customization_point{};

} // namespace query_impl
} // namespace std

#endif // STD_BITS_PROPERTIES_QUERY_H_INCLUDED
