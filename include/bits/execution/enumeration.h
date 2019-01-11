#ifndef STD_BITS_EXECUTION_ENUMERATION_H_INCLUDED
#define STD_BITS_EXECUTION_ENUMERATION_H_INCLUDED

#include <bits/properties/can_query.h>
#include <bits/execution/is_executor.h>
#include <type_traits>

namespace std {
namespace execution {
namespace impl {

template<class Enumeration, int N, int Value>
struct enumerator_impl;

template<class Enumeration, int N, class Enumerator>
struct default_enumerator
{
  template<class Executor>
  static constexpr auto static_query()
    -> decltype(Executor::query(*static_cast<Enumerator*>(0)))
  {
    return Executor::query(Enumerator());
  }

  template<class Executor, int I = 1>
  struct use_default_static_query :
    std::conditional_t<
      can_query<Executor, enumerator_impl<Enumeration, N, I> >::value,
      false_type,
      std::conditional_t<
        I + 1 < N,
        use_default_static_query<Executor, I + 1>,
        true_type
      >
    >
  {
  };

  template<class Executor>
  static constexpr auto static_query()
    -> std::enable_if_t<
      !std::impl::query_member_traits<Executor, Enumerator>::is_valid
        && use_default_static_query<Executor>::value,
      Enumerator
    >
  {
    return Enumerator();
  }
};

template<class Enumeration, int N, class Enumerator>
struct non_default_enumerator
{
  template<class Executor>
  static constexpr auto static_query()
    -> decltype(Executor::query(*static_cast<Enumerator*>(0)))
  {
    return Executor::query(Enumerator());
  }
};

template<class Enumeration, int N, int Value>
struct enumerator_impl :
  std::conditional_t<
    Value == 0,
    default_enumerator<Enumeration, N, enumerator_impl<Enumeration, N, Value> >,
    non_default_enumerator<Enumeration, N, enumerator_impl<Enumeration, N, Value> >
  >
{
  static constexpr bool is_requirable_concept = false;
  static constexpr bool is_requirable = true;
  static constexpr bool is_preferable = true;

  using base_type_helper =
    std::conditional_t<
      Value == 0,
      default_enumerator<Enumeration, N, enumerator_impl<Enumeration, N, Value> >,
      non_default_enumerator<Enumeration, N, enumerator_impl<Enumeration, N, Value> >
    >;

  using polymorphic_query_result_type = Enumeration;

  template<class Executor,
    class T = std::enable_if_t<
      (base_type_helper::template static_query<Executor>(), true),
      decltype(base_type_helper::template static_query<Executor>())
    >>
  static constexpr T static_query_v = base_type_helper::template static_query<Executor>();

  static constexpr Enumeration value()
  {
    return Enumeration(Value);
  }
};

template<class Enumeration, int N>
struct enumeration
{
  template<int I>
    using enumerator = enumerator_impl<Enumeration, N, I>;

  static constexpr bool is_requirable_concept = false;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = Enumeration;

  template<class Executor>
  static constexpr auto static_query() ->
    decltype(Executor::query(*static_cast<Enumeration*>(0)))
  {
    return Executor::query(Enumeration());
  }

  template<class Executor, int I = 0>
  struct static_query_type :
    std::conditional_t<
      std::impl::query_static_traits<Executor, enumerator<I> >::is_valid,
      std::decay<enumerator<I> >,
      std::conditional_t<
        I + 1 < N,
        static_query_type<Executor, I + 1>,
        std::decay<enable_if<false> >
      >
    >
  {
  };

  template<class Executor>
  static constexpr auto static_query()
    -> std::enable_if_t<
      !std::impl::query_member_traits<Executor, Enumeration>::is_valid,
      decltype(static_query_type<Executor>::type::template static_query_v<Executor>)
    >
  {
    return static_query_type<Executor>::type::template static_query_v<Executor>;
  }

  template<class Executor,
    class T = std::enable_if_t<
      (enumeration::static_query<Executor>(), true),
      decltype(enumeration::static_query<Executor>())
    >>
  static constexpr T static_query_v = enumeration::static_query<Executor>();

  constexpr enumeration()
    : value_(-1)
  {
  }

  template<int I, class = std::enable_if_t<I < N>>
  constexpr enumeration(enumerator<I>)
    : value_(enumerator<I>::value().value_)
  {
  }

  template<class Executor, int I = 0>
  struct query_type :
    std::conditional_t<
      can_query<Executor, enumerator<I> >::value,
      std::decay<enumerator<I> >,
      std::conditional_t<
        I + 1 < N,
        query_type<Executor, I + 1>,
        std::decay<enable_if<false> >
      >
    >
  {
  };

  template<class Executor, class Property,
    class = std::enable_if_t<std::is_same<Property, Enumeration>::value>>
  friend constexpr auto query(const Executor& ex, const Property&)
    noexcept(noexcept(std::query(ex, typename query_type<Executor>::type())))
    -> decltype(std::query(ex, typename query_type<Executor>::type()))
  {
    return std::query(ex, typename query_type<Executor>::type());
  }

  friend constexpr bool operator==(const Enumeration& a, const Enumeration& b) noexcept
  {
    return a.value_ == b.value_;
  }

  friend constexpr bool operator!=(const Enumeration& a, const Enumeration& b) noexcept
  {
    return a.value_ != b.value_;
  }

private:
  template<class, int, int> friend struct enumerator_impl;

  constexpr enumeration(int v)
    : value_(v)
  {
  }

  int value_;
};

} // namespace impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_ENUMERATION_H_INCLUDED
