#include <experimental/thread_pool>
#include <iostream>

namespace execution = std::experimental::execution;
using std::experimental::static_thread_pool;
using std::experimental::executors_v1::future;

int main()
{
  static_thread_pool pool{1};
  auto ex = execution::require(pool.executor(), execution::never_blocking);
  future<std::pair<int, std::exception_ptr>> f = ex.bulk_twoway_execute(
    [](
        int n,
        // Can't make atomic exception_ptr
        std::pair<int, std::exception_ptr>& result,
        int&){
      try {
        if(n == 5) {
          throw std::logic_error("Have a 5");
        } else {
          result.first += n;
        }
      } catch(...) {
        if(!result.second) {
          result.second = std::current_exception();
        }
      }
    },
    8,
    []() -> std::pair<int, std::exception_ptr>{ return {0, nullptr}; },
    []{ return 0; });

  // We don't need this if we have a good expected type that future can
  // construct from
  auto f2 = f.then([](future<std::pair<int, std::exception_ptr>> result) -> int {
      auto p = result.get();
      if(p.second) {
         std::rethrow_exception(p.second);
      } else {
        return p.first;
      }
    });

  try {
    auto p = f2.get();
    std::cout << "result is " << p << "\n";
  } catch(std::logic_error& l) {
    std::cout << "Exception: " << l.what() << "\n";
  }

}
