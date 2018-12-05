#ifndef STD_BITS_EXECUTION_CAN_PREFER_H_INCLUDED
#define STD_BITS_EXECUTION_CAN_PREFER_H_INCLUDED

#include <type_traits>
#include <tuple>
#include <utility>

namespace std {
namespace execution {
namespace can_prefer_impl {

template<class Executor, class Properties, class = std::void_t<>>
struct eval : std::false_type {};

template<class Executor, class... Properties>
struct eval<Executor, std::tuple<Properties...>,
  std::void_t<decltype(
    ::std::execution::prefer(std::declval<Executor>(), std::declval<Properties>()...)
  )>> : std::true_type {};

} // namespace can_prefer_impl

template<class Executor, class... Properties>
struct can_prefer : can_prefer_impl::eval<Executor, std::tuple<Properties...>> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_CAN_PREFER_H_INCLUDED
