#ifndef STD_EXPERIMENTAL_BITS_EXECUTOR_SHAPE_H
#define STD_EXPERIMENTAL_BITS_EXECUTOR_SHAPE_H

#include <cstddef>
#include <type_traits>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace executor_shape_impl {

template<class Executor, class = std::void_t<>>
struct eval
{
  using type = std::size_t;
};

template<class Executor>
struct eval<Executor, std::void_t<typename Executor::shape_type>>
{
  using type = typename decltype(execution::require(std::declval<const Executor&>(), execution::bulk_oneway))::shape_type;
};

} // namespace executor_shape_impl

template<class Executor>
struct executor_shape : executor_shape_impl::eval<Executor> {};

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_EXECUTOR_SHAPE_H
