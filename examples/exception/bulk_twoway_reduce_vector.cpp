#include <experimental/thread_pool>
#include <iostream>
#include <vector>

namespace execution = std::experimental::execution;
using std::experimental::static_thread_pool;
using std::experimental::executors_v1::future;

int main()
{
  static_thread_pool pool{1};
  auto ex = execution::require(pool.executor(), execution::blocking.never);
  future<std::pair<int, std::vector<std::exception_ptr>>> f = ex.bulk_twoway_execute(
    [](
        int n,
        // Can't make atomic exception_ptr
        std::pair<int, std::vector<std::exception_ptr>>& result,
        int&){
      try {
        if(n == 5) {
          throw std::logic_error("Have a 5");
        }
        if(n == 6) {
          throw std::logic_error("Big 6");
        }
        result.first += n;
      } catch(...) {
        result.second.push_back(std::current_exception());
      }
    },
    8,
    []() -> std::pair<int, std::vector<std::exception_ptr>>{ return {0, {}}; },
    []{ return 0; });

  auto p = f.get();
  std::cout << "result is " << p.first << "\n";
  std::cout << "with " << p.second.size() << " exceptions:\n";
  for(auto& ex : p.second) {
    try {
      std::rethrow_exception(ex);
    } catch(std::logic_error& l) {
      std::cout << "\tException: " << l.what() << "\n";
    }
  }
}
