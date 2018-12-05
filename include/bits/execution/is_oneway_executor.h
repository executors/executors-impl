#ifndef STD_BITS_EXECUTION_IS_ONEWAY_EXECUTOR_H_INCLUDED
#define STD_BITS_EXECUTION_IS_ONEWAY_EXECUTOR_H_INCLUDED

#include <bits/execution/is_executor.h>

namespace std {
namespace execution {
namespace is_oneway_executor_impl {

struct nullary_function
{
  void operator()() {}
};

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().execute(std::declval<nullary_function>()))>::value>::type,
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().execute(std::declval<nullary_function&>()))>::value>::type,
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().execute(std::declval<const nullary_function&>()))>::value>::type,
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().execute(std::declval<nullary_function&&>()))>::value>::type
	>> : is_executor_impl::eval<T> {};

} // namespace is_oneway_executor_impl

template<class Executor>
struct is_oneway_executor : is_oneway_executor_impl::eval<Executor> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_IS_ONEWAY_EXECUTOR_H_INCLUDED
