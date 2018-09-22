#ifndef STD_EXPERIMENTAL_BITS_IS_DEPENDENCY_EXECUTOR_H
#define STD_EXPERIMENTAL_BITS_IS_DEPENDENCY_EXECUTOR_H

#include <experimental/bits/is_executor.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace is_dependency_executor_impl {

struct dependency_function
{
  template<class... T> void operator()(T&&...) {}
};

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename std::enable_if<!std::is_same<void, decltype(std::declval<T&&>().execute(std::declval<dependency_function>()))>::value>::type,
    typename std::enable_if<!std::is_same<void, decltype(std::declval<T&&>().execute(std::declval<dependency_function&>()))>::value>::type,
    typename std::enable_if<!std::is_same<void, decltype(std::declval<T&&>().execute(std::declval<const dependency_function&>()))>::value>::type,
    typename std::enable_if<!std::is_same<void, decltype(std::declval<T&&>().execute(std::declval<dependency_function&&>()))>::value>::type
	>> : is_executor_impl::eval<T> {};

} // namespace is_dependency_executor_impl

template<class Executor>
struct is_dependency_executor : is_dependency_executor_impl::eval<Executor> {};

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_IS_DEPENDENCY_EXECUTOR_H
