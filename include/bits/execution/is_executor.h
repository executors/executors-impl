#ifndef STD_BITS_EXECUTION_IS_EXECUTOR_H_INCLUDED
#define STD_BITS_EXECUTION_IS_EXECUTOR_H_INCLUDED

#include <type_traits>
#include <utility>

namespace std {
namespace execution {
namespace is_executor_impl {

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename std::enable_if<std::is_nothrow_copy_constructible<T>::value>::type,
    typename std::enable_if<std::is_nothrow_move_constructible<T>::value>::type,
    typename std::enable_if<noexcept(static_cast<bool>(std::declval<const T&>() == std::declval<const T&>()))>::type,
    typename std::enable_if<noexcept(static_cast<bool>(std::declval<const T&>() != std::declval<const T&>()))>::type
	>> : std::true_type {};

} // namespace is_executor_impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_IS_EXECUTOR_H_INCLUDED
