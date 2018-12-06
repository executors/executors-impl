#ifndef STD_BITS_PROPERTIES_REQUIRE_CONCEPT_STATIC_TRAITS_H_INCLUDED
#define STD_BITS_PROPERTIES_REQUIRE_CONCEPT_STATIC_TRAITS_H_INCLUDED

#include <type_traits>

namespace std {
namespace impl {

template<class Entity, class Property, class = std::void_t<>>
struct require_concept_static_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Entity, class Property>
struct require_concept_static_traits<Entity, Property,
  std::enable_if_t<
    std::decay_t<Property>::value() == std::decay_t<Property>::template static_query_v<std::decay_t<Entity>>
  >>
{
  static constexpr bool is_valid = true;
  using result_type = std::decay_t<Entity>;
  static constexpr bool is_noexcept = noexcept(result_type(std::declval<Entity>()));
};

} // namespace impl
} // namespace std

#endif // STD_BITS_PROPERTIES_REQUIRE_CONCEPT_STATIC_TRAITS_H_INCLUDED
