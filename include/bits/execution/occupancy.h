#ifndef STD_BITS_EXECUTION_OCCUPANCY_H_INCLUDED
#define STD_BITS_EXECUTION_OCCUPANCY_H_INCLUDED

#include <cstddef>

namespace std {
namespace execution {
namespace occupancy_impl {

template<class Derived>
struct property_base
{
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = std::size_t;

  template<class Executor, class Type = decltype(Executor::query(*static_cast<Derived*>(0)))>
    static constexpr Type static_query_v = Executor::query(Derived());
};

} // end namespace occupancy_impl

struct occupancy_t : occupancy_impl::property_base<occupancy_t> {};

constexpr occupancy_t occupancy;

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_OCCUPANCY_H_INCLUDED
