#ifndef STD_EXPERIMENTAL_BITS_BULK_GUARANTEE_H
#define STD_EXPERIMENTAL_BITS_BULK_GUARANTEE_H

#include <experimental/bits/enumeration.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct bulk_guarantee_t :
  impl::enumeration<bulk_guarantee_t, 3>
{
  using impl::enumeration<bulk_guarantee_t, 3>::enumeration;

  using unsequenced_t = enumerator<0>;
  using sequenced_t = enumerator<1>;
  using parallel_t = enumerator<2>;

  static constexpr unsequenced_t unsequenced{};
  static constexpr sequenced_t sequenced{};
  static constexpr parallel_t parallel{};
};

constexpr bulk_guarantee_t bulk_guarantee{};
inline constexpr bulk_guarantee_t::unsequenced_t bulk_guarantee_t::unsequenced;
inline constexpr bulk_guarantee_t::sequenced_t bulk_guarantee_t::sequenced;
inline constexpr bulk_guarantee_t::parallel_t bulk_guarantee_t::parallel;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_BULK_GUARANTEE_H
