#ifndef STD_EXPERIMENTAL_BITS_PREFER_FREE_TRAITS_H
#define STD_EXPERIMENTAL_BITS_PREFER_FREE_TRAITS_H

#include <type_traits>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {
namespace impl {

template<class Executor, class Property, class = std::void_t<>>
struct prefer_free_traits
{
  static constexpr bool is_valid = false;
  static constexpr bool is_noexcept = false;
};

template<class Executor, class Property>
struct prefer_free_traits<Executor, Property,
  std::void_t<
    decltype(prefer(std::declval<Executor>(), std::declval<Property>()))
  >>
{
  static constexpr bool is_valid = true;
  using result_type = decltype(prefer(std::declval<Executor>(), std::declval<Property>()));
  static constexpr bool is_noexcept = noexcept(prefer(std::declval<Executor>(), std::declval<Property>()));
};

} // namespace impl
} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_PREFER_FREE_TRAITS_H
