#ifndef STD_EXPERIMENTAL_BITS_ENUMERATION_H
#define STD_EXPERIMENTAL_BITS_ENUMERATION_H

#include <experimental/bits/can_query.h>
#include <type_traits>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace impl {

template<class Derived, int N>
struct enumeration
{
  template<int Value, class>
  struct enumerator;

  template<class Enumerator>
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
        can_query<Executor, enumerator<I, Derived> >::value,
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
        !query_member_traits<Executor, Enumerator>::is_valid
          && use_default_static_query<Executor>::value,
        Enumerator
      >
    {
      return Enumerator();
    }
  };

  template<class Enumerator>
  struct non_default_enumerator
  {
    template<class Executor>
    static constexpr auto static_query()
      -> decltype(Executor::query(*static_cast<Enumerator*>(0)))
    {
      return Executor::query(Enumerator());
    }
  };

  template<int Value, class = Derived>
  struct enumerator :
    std::conditional_t<
      Value == 0,
      default_enumerator<enumerator<Value> >,
      non_default_enumerator<enumerator<Value> >
    >
  {
    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = true;

    using polymorphic_query_result_type = Derived;

    template<class Executor,
      class T = std::enable_if_t<
        (enumerator::template static_query<Executor>(), true),
        decltype(enumerator::template static_query<Executor>())
      >>
    static constexpr T static_query_v = enumerator::template static_query<Executor>();

    static constexpr Derived value()
    {
      return Derived(Value);
    }
  };

  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = Derived;

  template<class Executor>
  static constexpr auto static_query() ->
    decltype(Executor::query(*static_cast<Derived*>(0)))
  {
    return Executor::query(Derived());
  }

  template<class Executor, int I = 0>
  struct static_query_type :
    std::conditional_t<
      query_static_traits<Executor, enumerator<I> >::is_valid,
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
      !query_member_traits<Executor, Derived>::is_valid,
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
    class = std::enable_if_t<std::is_same<Property, Derived>::value>>
  friend constexpr auto query(const Executor& ex, const Property&)
    noexcept(noexcept(execution::query(ex, typename query_type<Executor>::type())))
    -> decltype(execution::query(ex, typename query_type<Executor>::type()))
  {
    return execution::query(ex, typename query_type<Executor>::type());
  }

  friend constexpr bool operator==(const Derived& a, const Derived& b) noexcept
  {
    return a.value_ == b.value_;
  }

  friend constexpr bool operator!=(const Derived& a, const Derived& b) noexcept
  {
    return a.value_ != b.value_;
  }

private:
  constexpr enumeration(int v)
    : value_(v)
  {
  }

  int value_;
};

} // namespace impl
} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_ENUMERATION_H
