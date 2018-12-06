#ifndef STD_BITS_PROPERTIES_REQUIRE_CONCEPT_MEMBER_TRAITS_H_INCLUDED
#define STD_BITS_PROPERTIES_REQUIRE_CONCEPT_MEMBER_TRAITS_H_INCLUDED

#include <type_traits>

namespace std {
namespace impl {

template<class Entity, class Property, class = std::void_t<>>
struct require_concept_member_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Entity, class Property>
struct require_concept_member_traits<Entity, Property,
  std::void_t<
    decltype(std::declval<Entity>().require_concept(std::declval<Property>()))
  >>
{
  static constexpr bool is_valid = true;
  using result_type = decltype(std::declval<Entity>().require_concept(std::declval<Property>()));
  static constexpr bool is_noexcept = noexcept(std::declval<Entity>().require_concept(std::declval<Property>()));
};

} // namespace impl
} // namespace std

#endif // STD_BITS_PROPERTIES_REQUIRE_CONCEPT_MEMBER_TRAITS_H_INCLUDED
