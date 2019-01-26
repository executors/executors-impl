#ifndef STD_BITS_EXECUTION_MAPPING_H_INCLUDED
#define STD_BITS_EXECUTION_MAPPING_H_INCLUDED

#include <bits/execution/enumeration.h>

namespace std {
namespace execution {

struct mapping_t :
  impl::enumeration<mapping_t, 3>
{
  using impl::enumeration<mapping_t, 3>::enumeration;

  using thread_t = enumerator<0>;
  using new_thread_t = enumerator<1>;
  using other_t = enumerator<2>;

  static constexpr thread_t thread{};
  static constexpr new_thread_t new_thread{};
  static constexpr other_t other{};
};

constexpr mapping_t mapping{};
inline constexpr mapping_t::thread_t mapping_t::thread;
inline constexpr mapping_t::new_thread_t mapping_t::new_thread;
inline constexpr mapping_t::other_t mapping_t::other;

} // namespace execution

template<class Entity>
struct is_applicable_property<Entity, execution::mapping_t,
  std::enable_if_t<execution::is_executor_v<Entity>>>
    : std::true_type {};

} // namespace std

#endif // STD_BITS_EXECUTION_MAPPING_H_INCLUDED
