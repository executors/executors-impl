#ifndef STD_BITS_PROPERTIES_CAN_PREFER_H_INCLUDED
#define STD_BITS_PROPERTIES_CAN_PREFER_H_INCLUDED

#include <type_traits>
#include <tuple>
#include <utility>

namespace std {
namespace can_prefer_impl {

template<class Entity, class Properties, class = std::void_t<>>
struct eval : std::false_type {};

template<class Entity, class... Properties>
struct eval<Entity, std::tuple<Properties...>,
  std::void_t<decltype(
    ::std::prefer(std::declval<Entity>(), std::declval<Properties>()...)
  )>> : std::true_type {};

} // namespace can_prefer_impl

template<class Entity, class... Properties>
struct can_prefer : can_prefer_impl::eval<Entity, std::tuple<Properties...>> {};

} // namespace std

#endif // STD_BITS_PROPERTIES_CAN_PREFER_H_INCLUDED
