#ifndef STD_EXPERIMENTAL_BITS_REQUIRE_STATIC_TRAITS_H
#define STD_EXPERIMENTAL_BITS_REQUIRE_STATIC_TRAITS_H

#include <type_traits>

namespace std {
namespace experimental {
inline namespace executors_v1 {
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
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_REQUIRE_STATIC_TRAITS_H
