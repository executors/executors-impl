#ifndef STD_BITS_EXECUTION_CAN_QUERY_H_INCLUDED
#define STD_BITS_EXECUTION_CAN_QUERY_H_INCLUDED

#include <type_traits>
#include <tuple>
#include <utility>

namespace std {
namespace execution {
namespace can_query_impl {

template<class Executor, class Properties, class = std::void_t<>>
struct eval : std::false_type {};

template<class Executor, class Property>
struct eval<Executor, std::tuple<Property>,
  std::void_t<decltype(
    ::std::execution::query(std::declval<Executor>(), std::declval<Property>())
  )>> : std::true_type {};

} // namespace can_query_impl

template<class Executor, class Property>
struct can_query : can_query_impl::eval<Executor, std::tuple<Property>> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_CAN_QUERY_H_INCLUDED
