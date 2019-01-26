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

static_assert(execution::is_oneway_executor_v<inline_executor>, "one way executor requirements not met");
static_assert(execution::is_bulk_oneway_executor_v<decltype(std::require_concept(inline_executor(), execution::bulk_oneway))>, "bulk one way executor requirements not met");

int main()
{
  inline_executor ex1;
  auto ex2 = std::require_concept(ex1, execution::bulk_oneway);
  ex2.bulk_execute([](int n, int&){ std::cout << "part " << n << "\n"; }, 8, []{ return 0; });
}
