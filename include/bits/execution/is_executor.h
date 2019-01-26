#ifndef STD_BITS_EXECUTION_IS_EXECUTOR_H_INCLUDED
#define STD_BITS_EXECUTION_IS_EXECUTOR_H_INCLUDED

#include <type_traits>
#include <utility>
#include <bits/execution/executor_concept.h>

namespace std {
namespace execution {
namespace is_executor_impl {

template<class T, class = std::void_t<>>
struct eval : std::false_type {};

template<class T>
struct eval<T,
  std::void_t<
    decltype(executor_concept_t::static_query_v<T>)
  >> : std::true_type {};

} // namespace is_executor_impl

template<class Executor>
struct is_executor : is_executor_impl::eval<Executor> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_IS_EXECUTOR_H_INCLUDED
