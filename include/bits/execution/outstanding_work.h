#ifndef STD_BITS_EXECUTION_OUTSTANDING_WORK_H_INCLUDED
#define STD_BITS_EXECUTION_OUTSTANDING_WORK_H_INCLUDED

#include <bits/execution/enumeration.h>

namespace std {
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

#if defined(__cpp_concepts)
template<execution::Executor E>
struct is_applicable_property<E, execution::outstanding_work_t> : std::true_type {};
#else
template<class Entity>
struct is_applicable_property<Entity, execution::outstanding_work_t,
  std::enable_if_t<execution::is_executor_v<Entity>>>
    : std::true_type {};
#endif

} // namespace std

#endif // STD_BITS_EXECUTION_OUTSTANDING_WORK_H_INCLUDED
