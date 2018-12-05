#include <thread_pool>
#include <iostream>

namespace execution = std::execution;
using std::static_thread_pool;

using executor = execution::executor<
  execution::oneway_t,
  execution::blocking_t::always_t,
  execution::blocking_t::never_t,
  std::prefer_only<execution::relationship_t::continuation_t>>;

int main()
{
  static_thread_pool pool{1};
  executor ex = std::require(pool.executor(), execution::blocking.always);
  std::cout << "before submission\n";
  ex.execute([ex = std::prefer(std::require(ex, execution::blocking.never), execution::relationship.continuation)]{
      std::cout << "outer starts\n";
      ex.execute([]{ std::cout << "inner\n"; });
      std::cout << "outer ends\n";
    });
  std::cout << "after submission, before wait\n";
  pool.wait();
  std::cout << "after wait\n";
}
