#include <execution>
#include <iostream>

namespace execution = std::execution;

class inline_executor
{
public:
  static constexpr auto query(execution::executor_concept_t) { return execution::oneway; }

  friend bool operator==(const inline_executor&, const inline_executor&) noexcept
  {
    return true;
  }

  friend bool operator!=(const inline_executor&, const inline_executor&) noexcept
  {
    return false;
  }

  template <class Function>
  void execute(Function f) const noexcept
  {
    f();
  }
};

#if defined(__cpp_concepts)
static_assert(execution::OneWayExecutor<inline_executor>, "one way executor concept not satisfied");
#else
static_assert(execution::is_oneway_executor_v<inline_executor>, "one way executor requirements not met");
#endif

int main()
{
  inline_executor ex;
  auto ex2 = std::require_concept(ex, execution::oneway);
  ex2.execute([]{ std::cout << "we made it\n"; });
}
