#include <chrono>
#include <iostream>
#include <thread>
#include <thread_pool>

namespace execution = std::execution;
using std::static_thread_pool;

using task_executor = execution::executor<
        execution::oneway_t,
        execution::blocking_t::never_t>;

using completion_executor = execution::executor<
        execution::oneway_t,
        execution::blocking_t::never_t,
        execution::prefer_only<execution::blocking_t::possibly_t>,
        execution::prefer_only<execution::outstanding_work_t::tracked_t>>;

// An operation that doubles a value asynchronously.
void my_twoway_operation_1(const task_executor& tex, int n,
    const completion_executor& cex, std::function<void(int)> h)
{
  if (n == 0)
  {
    // Nothing to do. Operation finishes immediately.
    // Specify non-blocking to prevent stack overflow.
    cex.require(execution::blocking.never).execute(
        [h = std::move(h), n]() mutable
        {
          h(n);
        });
  }
  else
  {
    // Simulate an asynchronous operation.
    tex.require(execution::blocking.never).execute(
        [n, cex = execution::prefer(cex, execution::outstanding_work.tracked), h = std::move(h)]() mutable
        {
          int result = n * 2;
          std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate long running work.
          execution::prefer(cex, execution::blocking.possibly).execute(
              [h = std::move(h), result]() mutable
              {
                h(result);
              });
        });
  }
}

struct my_twoway_operation_2_impl
{
  task_executor tex;
  int i, m;
  completion_executor cex;
  std::function<void(int)> h;

  void operator()(int n)
  {
    std::cout << "intermediate result is " << n << "\n";
    if (i < m)
    {
      ++i;
      my_twoway_operation_1(tex, n, cex, *this);
    }
    else
    {
      h(n);
    }
  }
};

void my_twoway_operation_2(const task_executor& tex, int n, int m,
    const completion_executor& cex, std::function<void(int)> h)
{
  // Intermediate steps of the composed operation are always continuations,
  // so we save the stored executors with that attribute rebound in.
  my_twoway_operation_1(tex, n, cex,
    my_twoway_operation_2_impl{
        execution::prefer(tex, execution::relationship.continuation), 0, m,
        execution::prefer(cex, execution::relationship.continuation), std::move(h)});
}

int main()
{
  static_thread_pool task_pool{1};
  static_thread_pool completion_pool{1};
  my_twoway_operation_2(task_pool.executor(), 21, 3, completion_pool.executor(),
      [](int n){ std::cout << "the answer is " << n << "\n"; });
  completion_pool.wait();
}
