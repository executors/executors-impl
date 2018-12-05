#include <thread_pool>
#include <iostream>

namespace execution = std::execution;
using std::static_thread_pool;

using executor = execution::executor<
  execution::oneway_t>;

int main()
{
  static_thread_pool pool{1};
  executor ex = std::require(pool.executor(), execution::blocking.possibly);
  ex.execute([]{ std::cout << "we made it\n"; });
  pool.wait();
}
