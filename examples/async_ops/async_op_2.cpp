#include <chrono>
#include <iostream>
#include <thread>
#include <thread_pool>

namespace execution = std::execution;
using std::static_thread_pool;

// An operation that doubles a value asynchronously.
template <class TaskExecutor, class CompletionExecutor, class CompletionHandler>
void my_twoway_operation_1(const TaskExecutor& tex, int n,
    const CompletionExecutor& cex, CompletionHandler h)
{
  if (n == 0)
  {
    // Nothing to do. Operation finishes immediately.
    // Specify non-blocking to prevent stack overflow.
    execution::require(cex,
        execution::oneway,
        execution::blocking.never
      ).execute(
        [h = std::move(h), n]() mutable
        {
          h(n);
        });
  }
  else
  {
    // Simulate an asynchronous operation.
    execution::require(tex,
        execution::oneway,
        execution::blocking.never
      ).execute(
        [n, cex = execution::prefer(cex, execution::outstanding_work.tracked), h = std::move(h)]() mutable
        {
          int result = n * 2;
          std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate long running work.
          execution::prefer(
              execution::require(cex, execution::oneway),
              execution::blocking.possibly
            ).execute(
              [h = std::move(h), result]() mutable
              {
                h(result);
              });
        });
  }
}

template <class TaskExecutor, class CompletionExecutor, class CompletionHandler>
struct my_twoway_operation_2_impl
{
  TaskExecutor tex;
  int i, m;
  CompletionExecutor cex;
  CompletionHandler h;

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

template <class TaskExecutor, class CompletionExecutor, class CompletionHandler>
void my_twoway_operation_2(const TaskExecutor& tex, int n, int m,
    const CompletionExecutor& cex, CompletionHandler h)
{
  // Intermediate steps of the composed operation are always continuations,
  // so we save the stored executors with that attribute rebound in.
  my_twoway_operation_1(tex, n, cex,
    my_twoway_operation_2_impl<decltype(execution::prefer(tex, execution::relationship.continuation)),
      decltype(execution::prefer(cex, execution::relationship.continuation)), CompletionHandler>{
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
