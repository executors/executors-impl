#ifndef STD_EXPERIMENTAL_BITS_REQUIRE_MEMBER_TRAITS_H
#define STD_EXPERIMENTAL_BITS_REQUIRE_MEMBER_TRAITS_H

#include <type_traits>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace impl {

template<class Executor, class Property, class = std::void_t<>>
struct require_member_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Executor, class Property>
struct require_member_traits<Executor, Property,
  std::void_t<
    decltype(std::declval<Executor>().require(std::declval<Property>()))
  >>
{
  static constexpr bool is_valid = true;
  using result_type = decltype(std::declval<Executor>().require(std::declval<Property>()));
  static constexpr bool is_noexcept = noexcept(std::declval<Executor>().require(std::declval<Property>()));
};

} // namespace impl
} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_REQUIRE_MEMBER_TRAITS_H
