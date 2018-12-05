#include <execution>
#include <iostream>

namespace execution = std::execution;

class inline_executor
{
public:
  static constexpr execution::blocking_t query(execution::blocking_t) { return execution::blocking.always; }

  friend bool operator==(const inline_executor&, const inline_executor&) noexcept
  {
    return true;
  }

  friend bool operator!=(const inline_executor&, const inline_executor&) noexcept
  {
    return false;
  }

  template <class Function, class SharedFactory>
  void bulk_execute(Function f, std::size_t n, SharedFactory sf) const noexcept
  {
    auto shared_state(sf());
    for (std::size_t i = 0; i < n; ++i)
      f(i, shared_state);
  }
};

static_assert(execution::is_bulk_oneway_executor_v<inline_executor>, "bulk one way executor requirements not met");
static_assert(!execution::is_oneway_executor_v<inline_executor>, "must not meet one way executor requirements");

int main()
{
  inline_executor ex1;
  auto ex2 = std::require(ex1, execution::blocking.always);
  ex2.bulk_execute([](int n, int&){ std::cout << "part " << n << "\n"; }, 8, []{ return 0; });
}
