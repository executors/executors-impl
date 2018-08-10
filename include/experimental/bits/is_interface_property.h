#ifndef STD_EXPERIMENTAL_BITS_IS_INTERFACE_PROPERTY_H
#define STD_EXPERIMENTAL_BITS_IS_INTERFACE_PROPERTY_H

#include <type_traits>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace is_interface_property_impl {

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename T::template polymorphic_executor_type<>
	>> : std::true_type {};

} // namespace is_interface_property_impl

template<class Executor>
struct is_interface_property : is_interface_property_impl::eval<Executor> {};

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_IS_INTERFACE_PROPERTY_H
