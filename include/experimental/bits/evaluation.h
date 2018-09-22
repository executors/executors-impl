#ifndef STD_EXPERIMENTAL_BITS_EVALUATION_H
#define STD_EXPERIMENTAL_BITS_EVALUATION_H

#include <experimental/bits/enumeration.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct evaluation_t :
  impl::enumeration<evaluation_t, 3>
{
  using impl::enumeration<evaluation_t, 3>::enumeration;

  using eager_t = enumerator<0>;
  using lazy_t = enumerator<1>;

  static constexpr eager_t eager{};
  static constexpr lazy_t lazy{};
};

constexpr evaluation_t evaluation{};
inline constexpr evaluation_t::eager_t evaluation_t::eager;
inline constexpr evaluation_t::lazy_t evaluation_t::lazy;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_EVALUATION_H
