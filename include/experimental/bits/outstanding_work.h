#ifndef STD_EXPERIMENTAL_BITS_OUTSTANDING_WORK_H
#define STD_EXPERIMENTAL_BITS_OUTSTANDING_WORK_H

#include <experimental/bits/enumeration.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct outstanding_work_t :
  impl::enumeration<outstanding_work_t, 2>
{
  using impl::enumeration<outstanding_work_t, 2>::enumeration;

  using untracked_t = enumerator<0>;
  using tracked_t = enumerator<1>;

  static constexpr untracked_t untracked{};
  static constexpr tracked_t tracked{};
};

constexpr outstanding_work_t outstanding_work{};
inline constexpr outstanding_work_t::untracked_t outstanding_work_t::untracked;
inline constexpr outstanding_work_t::tracked_t outstanding_work_t::tracked;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_OUTSTANDING_WORK_H
