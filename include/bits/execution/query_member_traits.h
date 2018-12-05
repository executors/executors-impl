#ifndef STD_BITS_EXECUTION_QUERY_MEMBER_TRAITS_H_INCLUDED
#define STD_BITS_EXECUTION_QUERY_MEMBER_TRAITS_H_INCLUDED

#include <type_traits>

namespace std {
namespace execution {
namespace impl {

template<class Executor, class Property, class = std::void_t<>>
struct query_member_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Executor, class Property>
struct query_member_traits<Executor, Property,
  std::void_t<
    decltype(std::declval<Executor>().query(std::declval<Property>()))
  >>
{
  static constexpr bool is_valid = true;
  using result_type = decltype(std::declval<Executor>().query(std::declval<Property>()));
  static constexpr bool is_noexcept = noexcept(std::declval<Executor>().query(std::declval<Property>()));
};

} // namespace impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_QUERY_MEMBER_TRAITS_H_INCLUDED
