#ifndef STD_BITS_PROPERTIES_CAN_REQUIRE_CONCEPT_H_INCLUDED
#define STD_BITS_PROPERTIES_CAN_REQUIRE_CONCEPT_H_INCLUDED

#include <type_traits>
#include <tuple>
#include <utility>

namespace std {
namespace can_require_concept_impl {

template<class Entity, class Properties, class = std::void_t<>>
struct eval : std::false_type {};

template<class Entity, class Property>
struct eval<Entity, std::tuple<Property>,
  std::void_t<decltype(
    ::std::require_concept(std::declval<Entity>(), std::declval<Property>())
  )>> : std::true_type {};

} // namespace can_require_concept_impl

template<class Entity, class Property>
struct can_require_concept : can_require_concept_impl::eval<Entity, std::tuple<Property>> {};

} // namespace std

#endif // STD_BITS_PROPERTIES_CAN_REQUIRE_CONCEPT_H_INCLUDED
