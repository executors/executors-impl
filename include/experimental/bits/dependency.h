#ifndef STD_EXPERIMENTAL_BITS_DEPENDENCY_H
#define STD_EXPERIMENTAL_BITS_DEPENDENCY_H

#include <experimental/bits/is_dependency_executor.h>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

template<class SenderExecutor>
struct dependency_t
{
  static constexpr bool is_requirable = true;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  //template<class... SupportableProperties>
  //  class polymorphic_executor_type;

  //template<class Executor>
  //  static constexpr bool static_query_v
  //    = is_dependency_executor<Executor>::value;

  //static constexpr bool value() { return true; }
  SenderExecutor executor() const && noexcept { return std::move(executor_); }

  explicit dependency_t(SenderExecutor executor)
    : executor_(std::move(executor))
  {
  }

private:
  SenderExecutor executor_;
};

template<>
struct dependency_t<void>
{
  static constexpr bool is_requirable = true;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  //template<class... SupportableProperties>
  //  class polymorphic_executor_type;

  template<class Executor>
    static constexpr bool static_query_v
      = is_dependency_executor<Executor>::value;

  static constexpr bool value() { return true; }
};

constexpr dependency_t<void> dependency;

template<typename SenderExecutor>
constexpr dependency_t<SenderExecutor> depends_on(SenderExecutor executor)
{
  return dependency_t<SenderExecutor>(std::move(executor));
}

constexpr dependency_t<void> depends_on()
{
  return dependency_t<void>();
}

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_DEPENDENCY_H
