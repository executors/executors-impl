#include <thread_pool>
#include <iostream>

namespace execution = std::execution;
using std::static_thread_pool;

using executor = execution::executor<
  execution::bulk_oneway_t,
  execution::blocking_t::always_t>;

int main()
{
  static_thread_pool pool{1};
  executor ex = std::require(pool.executor(), execution::bulk_oneway, execution::blocking.always);
  ex.bulk_execute([](int n, int&){ std::cout << "part " << n << "\n"; }, 8, []{ return 0; });
}
