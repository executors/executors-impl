#ifndef STD_BITS_EXECUTION_IS_BULK_ONEWAY_EXECUTOR_H_INCLUDED
#define STD_BITS_EXECUTION_IS_BULK_ONEWAY_EXECUTOR_H_INCLUDED

#include <bits/execution/is_executor.h>

namespace std {
namespace execution {
namespace is_bulk_oneway_executor_impl {

struct shared_state {};

struct shared_factory
{
  shared_state operator()() const { return {}; }
};

struct bulk_function
{
  void operator()(std::size_t, shared_state&) {}
};

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().bulk_execute(
          std::declval<bulk_function>(), 1, std::declval<shared_factory>()))>::value>::type,
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().bulk_execute(
          std::declval<bulk_function&>(), 1, std::declval<shared_factory>()))>::value>::type,
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().bulk_execute(
          std::declval<const bulk_function&>(), 1, std::declval<shared_factory>()))>::value>::type,
    typename std::enable_if<std::is_same<void, decltype(std::declval<const T&>().bulk_execute(
          std::declval<bulk_function&&>(), 1, std::declval<shared_factory>()))>::value>::type
	>> : is_executor_impl::eval<T> {};

} // namespace is_bulk_oneway_executor_impl

template<class Executor>
struct is_bulk_oneway_executor : is_bulk_oneway_executor_impl::eval<Executor> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_IS_BULK_ONEWAY_EXECUTOR_H_INCLUDED
