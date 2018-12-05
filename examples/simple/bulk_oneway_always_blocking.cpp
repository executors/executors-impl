#include <iostream>
#include <thread_pool>

namespace execution = std::execution;
using std::static_thread_pool;

int main()
{
  static_thread_pool pool{4};
  auto ex = std::require(pool.executor(), execution::bulk_oneway, execution::blocking.always);
  ex.bulk_execute([](int n, int&){ std::cout << "part " << n << "\n"; }, 8, []{ return 0; });
}
