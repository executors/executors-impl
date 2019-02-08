#ifndef STD_BITS_EXECUTION_IS_INTERFACE_PROPERTY_H_INCLUDED
#define STD_BITS_EXECUTION_IS_INTERFACE_PROPERTY_H_INCLUDED

#include <type_traits>

namespace std {
namespace execution {
namespace is_interface_property_impl {

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename T::template polymorphic_executor_type<>
	>> : std::integral_constant<bool, T::is_requirable_concept> {};

} // namespace is_interface_property_impl

template<class T>
struct is_interface_property : is_interface_property_impl::eval<T> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_IS_INTERFACE_PROPERTY_H_INCLUDED
