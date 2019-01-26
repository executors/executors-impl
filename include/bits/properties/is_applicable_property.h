#ifndef STD_BITS_PROPERTIES_IS_APPLICABLE_PROPERTY_H_INCLUDED
#define STD_BITS_PROPERTIES_IS_APPLICABLE_PROPERTY_H_INCLUDED

#include <type_traits>

namespace std {

#if defined(__cpp_concepts)
template<class Entity, class Property>
struct is_applicable_property : std::false_type {};
#else
template<class Entity, class Property, class>
struct is_applicable_property : std::false_type {};
#endif

} // namespace std

#endif // STD_BITS_PROPERTIES_IS_APPLICABLE_PROPERTY_H_INCLUDED
