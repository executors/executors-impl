#ifndef STD_BITS_EXECUTION_EXECUTOR_INDEX_H_INCLUDED
#define STD_BITS_EXECUTION_EXECUTOR_INDEX_H_INCLUDED

#include <bits/execution/executor_shape.h>
#include <type_traits>

namespace std {
namespace execution {
namespace executor_index_impl {

template<class Executor, class = std::void_t<>>
struct eval
{
  using type = executor_shape_t<Executor>;
};

template<class Executor>
struct eval<Executor, std::void_t<typename Executor::index_type>>
{
  using type = typename decltype(std::require(std::declval<const Executor&>(), execution::bulk_oneway))::index_type;
};

} // namespace executor_index_impl

template<class Executor>
struct executor_index : executor_index_impl::eval<Executor> {};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_EXECUTOR_INDEX_H_INCLUDED
