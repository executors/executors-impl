#ifndef STD_BITS_EXECUTION_QUERY_STATIC_TRAITS_H_INCLUDED
#define STD_BITS_EXECUTION_QUERY_STATIC_TRAITS_H_INCLUDED

#include <type_traits>

namespace std {
namespace execution {
namespace impl {

template<class Executor, class Property, class = std::void_t<>>
struct query_static_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Executor, class Property>
struct query_static_traits<Executor, Property,
  std::void_t<
    decltype(std::decay_t<Property>::template static_query_v<std::decay_t<Executor>>)
  >>
{
  static constexpr bool is_valid = true;
  using result_type = decltype(std::decay_t<Property>::template static_query_v<std::decay_t<Executor>>);
  static constexpr bool is_noexcept = true;
};

} // namespace impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_QUERY_STATIC_TRAITS_H_INCLUDED
