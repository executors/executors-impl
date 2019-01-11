#ifndef STD_BITS_EXECUTION_CONTEXT_H_INCLUDED
#define STD_BITS_EXECUTION_CONTEXT_H_INCLUDED

#include <bits/execution/is_executor.h>

namespace std {
namespace execution {
namespace context_impl {

template<class Derived>
struct property_base
{
  static constexpr bool is_requirable_concept = false;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  //using polymorphic_query_result_type = ?;

  template<class Executor, class Type = decltype(Executor::query(*static_cast<Derived*>(0)))>
    static constexpr Type static_query_v = Executor::query(Derived());
};

} // namespace context_impl

struct context_t : context_impl::property_base<context_t> {};

constexpr context_t context;

} // namespace execution

template<class Entity>
struct is_applicable_property<Entity, execution::context_t,
  std::enable_if_t<execution::is_oneway_executor_v<Entity> || execution::is_bulk_oneway_executor_v<Entity>>>
    : std::true_type {};

} // namespace std

#endif // STD_BITS_EXECUTION_CONTEXT_H_INCLUDED
