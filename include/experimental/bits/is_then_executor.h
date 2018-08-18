#ifndef STD_EXPERIMENTAL_BITS_IS_THEN_EXECUTOR_H
#define STD_EXPERIMENTAL_BITS_IS_THEN_EXECUTOR_H

#include <experimental/bits/is_executor.h>
#include <experimental/future>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace is_then_executor_impl {

struct then_function
{
  template<class T> void operator()(T&&) {}
};

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename std::enable_if<!std::is_same<void, decltype(std::declval<const T&>().then_execute(std::declval<then_function>(), std::declval<future<int>>()))>::value>::type,
    typename std::enable_if<!std::is_same<void, decltype(std::declval<const T&>().then_execute(std::declval<then_function&>(), std::declval<future<int>>()))>::value>::type,
    typename std::enable_if<!std::is_same<void, decltype(std::declval<const T&>().then_execute(std::declval<const then_function&>(), std::declval<future<int>>()))>::value>::type,
    typename std::enable_if<!std::is_same<void, decltype(std::declval<const T&>().then_execute(std::declval<then_function&&>(), std::declval<future<int>>()))>::value>::type
	>> : is_executor_impl::eval<T> {};

} // namespace is_then_executor_impl

template<class Executor>
struct is_then_executor : is_then_executor_impl::eval<Executor> {};

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_IS_THEN_EXECUTOR_H
