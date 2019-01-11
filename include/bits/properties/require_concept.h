#ifndef STD_BITS_PROPERTIES_REQUIRE_CONCEPT_H_INCLUDED
#define STD_BITS_PROPERTIES_REQUIRE_CONCEPT_H_INCLUDED

#include <bits/properties/can_query.h>
#include <bits/properties/require_concept_free_traits.h>
#include <bits/properties/require_concept_member_traits.h>
#include <bits/properties/require_concept_static_traits.h>
#include <utility>

namespace std {
namespace require_concept_impl {

struct require_concept_fn
{
  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&&) const
    -> std::enable_if_t<
      std::decay_t<Property>::is_requirable_concept
        && can_query<std::decay_t<Entity>, std::decay_t<Property>>::value
        && impl::require_concept_static_traits<Entity, Property>::is_valid,
      typename impl::require_concept_static_traits<Entity, Property>::result_type
    >
  {
    return std::forward<Entity>(ex);
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&& p) const
    noexcept(impl::require_concept_member_traits<Entity, Property>::is_noexcept)
    -> std::enable_if_t<
      std::decay_t<Property>::is_requirable_concept
        && can_query<std::decay_t<Entity>, std::decay_t<Property>>::value
        && !impl::require_concept_static_traits<Entity, Property>::is_valid
        && impl::require_concept_member_traits<Entity, Property>::is_valid,
      typename impl::require_concept_member_traits<Entity, Property>::result_type
    >
  {
    static_assert(
      impl::require_concept_static_traits<
        typename impl::require_concept_member_traits<Entity, Property>::result_type,
          Property>::is_valid, "concept requirements not met");
    return std::forward<Entity>(ex).require_concept(std::forward<Property>(p));
  }

  template<class Entity, class Property>
  constexpr auto operator()(Entity&& ex, Property&& p) const
    noexcept(impl::require_concept_free_traits<Entity, Property>::is_noexcept)
    -> std::enable_if_t<
      std::decay_t<Property>::is_requirable_concept
        && can_query<std::decay_t<Entity>, std::decay_t<Property>>::value
        && !impl::require_concept_static_traits<Entity, Property>::is_valid
        && !impl::require_concept_member_traits<Entity, Property>::is_valid
        && impl::require_concept_free_traits<Entity, Property>::is_valid,
      typename impl::require_concept_free_traits<Entity, Property>::result_type
    >
  {
    static_assert(
      impl::require_concept_static_traits<
        typename impl::require_concept_free_traits<Entity, Property>::result_type,
          Property>::is_valid, "concept requirements not met");
    return require_concept(std::forward<Entity>(ex), std::forward<Property>(p));
  }
};

template<class T = require_concept_fn> constexpr T customization_point{};

} // namespace require_concept_impl
} // namespace std

#endif // STD_BITS_PROPERTIES_REQUIRE_CONCEPT_H_INCLUDED
