#ifndef STD_BITS_EXECUTION_EXECUTOR_SHAPE_H_INCLUDED
#define STD_BITS_EXECUTION_EXECUTOR_SHAPE_H_INCLUDED

#include <cstddef>
#include <type_traits>

namespace std {
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
  using type = typename decltype(std::require_concept(std::declval<const Executor&>(), execution::bulk_oneway))::shape_type;
};

} // namespace executor_shape_impl

template<class Executor>
struct executor_shape : executor_shape_impl::eval<Executor> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_EXECUTOR_SHAPE_H_INCLUDED
