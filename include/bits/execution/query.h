#ifndef STD_BITS_EXECUTION_QUERY_H_INCLUDED
#define STD_BITS_EXECUTION_QUERY_H_INCLUDED

#include <bits/execution/query_free_traits.h>
#include <bits/execution/query_member_traits.h>
#include <bits/execution/query_static_traits.h>
#include <utility>

namespace std {
namespace execution {
namespace query_impl {

struct query_fn
{
  template<class Executor, class Property>
  constexpr auto operator()(Executor&&, Property&&) const
    -> std::enable_if_t<
      impl::query_static_traits<Executor, Property>::is_valid,
      typename impl::query_static_traits<Executor, Property>::result_type
    >
  {
    return std::decay_t<Property>::template static_query_v<std::decay_t<Executor>>;
  }

  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&& p) const
    noexcept(impl::query_member_traits<Executor, Property>::is_noexcept)
    -> std::enable_if_t<
      !impl::query_static_traits<Executor, Property>::is_valid
        && impl::query_member_traits<Executor, Property>::is_valid,
      typename impl::query_member_traits<Executor, Property>::result_type
    >
  {
    return std::forward<Executor>(ex).query(std::forward<Property>(p));
  }

  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&& p) const
    noexcept(impl::query_free_traits<Executor, Property>::is_noexcept)
    -> std::enable_if_t<
      !impl::query_static_traits<Executor, Property>::is_valid
        && !impl::query_member_traits<Executor, Property>::is_valid
        && impl::query_free_traits<Executor, Property>::is_valid,
      typename impl::query_free_traits<Executor, Property>::result_type
    >
  {
    return query(std::forward<Executor>(ex), std::forward<Property>(p));
  }
};

template<class T = query_fn> constexpr T customization_point{};

} // namespace query_impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_QUERY_H_INCLUDED
