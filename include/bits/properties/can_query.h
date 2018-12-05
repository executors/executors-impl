#ifndef STD_BITS_PROPERTIES_CAN_QUERY_H_INCLUDED
#define STD_BITS_PROPERTIES_CAN_QUERY_H_INCLUDED

#include <type_traits>
#include <tuple>
#include <utility>

namespace std {
namespace can_query_impl {

template<class Entity, class Properties, class = std::void_t<>>
struct eval : std::false_type {};

template<class Entity, class Property>
struct eval<Entity, std::tuple<Property>,
  std::void_t<decltype(
    ::std::query(std::declval<Entity>(), std::declval<Property>())
  )>> : std::true_type {};

} // namespace can_query_impl

template<class Entity, class Property>
struct can_query : can_query_impl::eval<Entity, std::tuple<Property>> {};

} // namespace std

#endif // STD_BITS_PROPERTIES_CAN_QUERY_H_INCLUDED
