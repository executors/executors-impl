#include <thread_pool>
#include <iostream>

namespace execution = std::execution;
using std::static_thread_pool;

int main()
{
  static_thread_pool pool{1};
  auto ex = std::require(pool.executor(), execution::blocking.always);
  std::cout << "before submission\n";
  ex.execute([ex]{
      std::cout << "outer starts\n";
      ex.execute([]{ std::cout << "inner\n"; });
      std::cout << "outer ends\n";
    });
  std::cout << "after submission, before wait\n";
  pool.wait();
  std::cout << "after wait\n";
}
