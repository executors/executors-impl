#ifndef STD_EXPERIMENTAL_BITS_MAPPING_H
#define STD_EXPERIMENTAL_BITS_MAPPING_H

#include <experimental/bits/enumeration.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
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
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_MAPPING_H
