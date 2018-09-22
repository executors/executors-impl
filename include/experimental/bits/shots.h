#ifndef STD_EXPERIMENTAL_BITS_SHOTS_H
#define STD_EXPERIMENTAL_BITS_SHOTS_H

#include <experimental/bits/enumeration.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct shots_t :
  impl::enumeration<shots_t, 3>
{
  using impl::enumeration<shots_t, 3>::enumeration;

  using single_t = enumerator<0>;
  using multi_t = enumerator<1>;

  static constexpr single_t single{};
  static constexpr multi_t multi{};
};

constexpr shots_t shots{};
inline constexpr shots_t::single_t shots_t::single;
inline constexpr shots_t::multi_t shots_t::multi;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_SHOTS_H
