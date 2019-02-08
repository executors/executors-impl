#ifndef STD_BITS_EXECUTION_ADAPTER_H_INCLUDED
#define STD_BITS_EXECUTION_ADAPTER_H_INCLUDED

#include <bits/properties/query_member_traits.h>
#include <bits/properties/query_static_member_traits.h>
#include <bits/properties/require_member_traits.h>
#include <bits/properties/require_concept_member_traits.h>

namespace std {
namespace execution {
namespace impl {

template<template<class> class Derived, class Executor>
class adapter
{
public:
  adapter(Executor ex)
    : executor_(std::move(ex))
  {
  }

  template<class Property>
  constexpr auto require_concept(const Property& p) const
    noexcept(std::impl::require_concept_member_traits<Executor, Property>::is_noexcept)
    -> Derived<typename std::impl::require_concept_member_traits<Executor, Property>::result_type>
  {
    return Derived<decltype(executor_.require_concept(p))>(executor_.require_concept(p));
  }

  template<class Property>
  constexpr auto require(const Property& p) const
    noexcept(std::impl::require_member_traits<Executor, Property>::is_noexcept)
    -> Derived<typename std::impl::require_member_traits<Executor, Property>::result_type>
  {
    return Derived<decltype(executor_.require(p))>(executor_.require(p));
  }

  template<class Property>
  static constexpr auto query(const Property& p)
    noexcept(std::impl::query_static_member_traits<Executor, Property>::is_noexcept)
    -> typename std::impl::query_static_member_traits<Executor, Property>::result_type
  {
    return Executor::query(p);
  }

  template<class Property>
  constexpr auto query(const Property& p) const
    noexcept(std::impl::query_member_traits<Executor, Property>::is_noexcept)
    -> typename enable_if<
      !std::impl::query_static_member_traits<Executor, Property>::is_valid,
      typename std::impl::query_member_traits<Executor, Property>::result_type
    >::type
  {
    return executor_.query(p);
  }

  friend constexpr bool operator==(const Derived<Executor>& a,
      const Derived<Executor>& b) noexcept
  {
    return a.executor_ == b.executor_;
  }

  friend constexpr bool operator!=(const Derived<Executor>& a,
      const Derived<Executor>& b) noexcept
  {
    return a.executor_ != b.executor_;
  }

protected:
  Executor executor_;
};

} // namespace impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_ADAPTER_H_INCLUDED
