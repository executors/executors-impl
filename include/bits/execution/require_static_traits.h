#ifndef STD_BITS_EXECUTION_REQUIRE_STATIC_TRAITS_H_INCLUDED
#define STD_BITS_EXECUTION_REQUIRE_STATIC_TRAITS_H_INCLUDED

#include <type_traits>

namespace std {
namespace execution {
namespace impl {

template<class Executor, class Property, class = std::void_t<>>
struct require_static_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Executor, class Property>
struct require_static_traits<Executor, Property,
  std::enable_if_t<
    std::decay_t<Property>::value() == std::decay_t<Property>::template static_query_v<std::decay_t<Executor>>
  >>
{
  static constexpr bool is_valid = true;
  using result_type = std::decay_t<Executor>;
  static constexpr bool is_noexcept = noexcept(result_type(std::declval<Executor>()));
};

} // namespace impl
} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_REQUIRE_STATIC_TRAITS_H_INCLUDED
