#ifndef STD_BITS_EXECUTION_ENUMERATOR_ADAPTER_H_INCLUDED
#define STD_BITS_EXECUTION_ENUMERATOR_ADAPTER_H_INCLUDED

#include <bits/execution/adapter.h>
#include <bits/properties/require.h>

namespace std {
namespace execution {
namespace impl {

template<template<class> class Derived, class Executor, class Enumeration, class Enumerator>
class enumerator_adapter : public adapter<Derived, Executor>
{
public:
  using adapter<Derived, Executor>::adapter;
  using adapter<Derived, Executor>::require;
  using adapter<Derived, Executor>::query;

  template<int N>
  constexpr auto require(const typename Enumeration::template enumerator<N>& p) const
    -> decltype(std::require(declval<Executor>(), p))
  {
    return std::require(this->executor_, p);
  }

  static constexpr Enumeration query(const Enumeration&) noexcept
  {
    return Enumerator{};
  }

  template<int N>
  static constexpr Enumeration query(const typename Enumeration::template enumerator<N>&) noexcept
  {
    return Enumerator{};
  }
};

} // namespace impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_ENUMERATOR_ADAPTER_H_INCLUDED
