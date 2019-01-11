#ifndef STD_BITS_PROPERTIES_IS_APPLICABLE_PROPERTY_H_INCLUDED
#define STD_BITS_PROPERTIES_IS_APPLICABLE_PROPERTY_H_INCLUDED

#include <type_traits>

namespace std {

template<class Entity, class Properties, class>
struct is_applicable_property : std::false_type {};

} // namespace std

#endif // STD_BITS_PROPERTIES_IS_APPLICABLE_PROPERTY_H_INCLUDED
