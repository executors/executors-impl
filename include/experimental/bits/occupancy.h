#ifndef STD_EXPERIMENTAL_BITS_OCCUPANCY_H
#define STD_EXPERIMENTAL_BITS_OCCUPANCY_H

#include <cstddef>

namespace std {
namespace experimental {
inline namespace executors_v1 {
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
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_OCCUPANCY_H
