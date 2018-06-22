#ifndef STD_EXPERIMENTAL_BITS_ENUMERATOR_ADAPTER_H
#define STD_EXPERIMENTAL_BITS_ENUMERATOR_ADAPTER_H

#include <experimental/bits/adapter.h>
#include <experimental/bits/require.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
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
    -> decltype(execution::require(declval<Executor>(), p))
  {
    return execution::require(this->executor_, p);
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
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_ENUMERATOR_ADAPTER_H
