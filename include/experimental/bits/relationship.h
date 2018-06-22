#ifndef STD_EXPERIMENTAL_BITS_RELATIONSHIP_H
#define STD_EXPERIMENTAL_BITS_RELATIONSHIP_H

#include <experimental/bits/enumeration.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct relationship_t :
  impl::enumeration<relationship_t, 2>
{
  using impl::enumeration<relationship_t, 2>::enumeration;

  using fork_t = enumerator<0>;
  using continuation_t = enumerator<1>;

  static constexpr fork_t fork{};
  static constexpr continuation_t continuation{};
};

constexpr relationship_t relationship{};
inline constexpr relationship_t::fork_t relationship_t::fork;
inline constexpr relationship_t::continuation_t relationship_t::continuation;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_RELATIONSHIP_H
