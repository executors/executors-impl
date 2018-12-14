#ifndef STD_BITS_EXECUTION_ONEWAY_H_INCLUDED
#define STD_BITS_EXECUTION_ONEWAY_H_INCLUDED

#include <bits/execution/is_executor.h>
#include <bits/execution/is_oneway_executor.h>

namespace std {
namespace execution {

struct oneway_t
{
  static constexpr bool is_requirable_concept = true;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  template<class... SupportableProperties>
    class polymorphic_executor_type;

  template<class Executor>
    static constexpr bool static_query_v
      = is_oneway_executor<Executor>::value;

  static constexpr bool value() { return true; }
};

constexpr oneway_t oneway;

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_ONEWAY_H_INCLUDED
