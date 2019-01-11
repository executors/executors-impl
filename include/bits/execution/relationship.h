#ifndef STD_BITS_EXECUTION_RELATIONSHIP_H_INCLUDED
#define STD_BITS_EXECUTION_RELATIONSHIP_H_INCLUDED

#include <bits/execution/enumeration.h>

namespace std {
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

template<class Entity>
struct is_applicable_property<Entity, execution::relationship_t,
  std::enable_if_t<execution::is_oneway_executor_v<Entity> || execution::is_bulk_oneway_executor_v<Entity>>>
    : std::true_type {};

} // namespace std

#endif // STD_BITS_EXECUTION_RELATIONSHIP_H_INCLUDED
