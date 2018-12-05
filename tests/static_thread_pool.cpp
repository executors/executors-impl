#include <thread_pool>

namespace execution = std::execution;
using std::static_thread_pool;

template<class Executor>
void static_thread_pool_executor_compile_test(Executor ex1)
{
  const Executor& cex1 = ex1;

  static_assert(noexcept(Executor(ex1)), "copy constructor must not throw");
  static_assert(noexcept(Executor(cex1)), "copy constructor must not throw");
  static_assert(noexcept(Executor(std::move(ex1))), "move constructor must not throw");

  Executor ex2(cex1);
  Executor ex3(std::move(ex1));

  static_assert(noexcept(ex2 = cex1), "copy assignment must not throw");
  static_assert(noexcept(ex3 = std::move(ex1)), "move assignment must not throw");

  ex2 = cex1;
  ex3 = std::move(ex1);

  bool b1 = cex1.running_in_this_thread();
  (void)b1;

  static_thread_pool& pool = execution::query(cex1, execution::context);
  (void)pool;

  const Executor& cex2 = ex2;

  bool b2 = (cex1 == cex2);
  (void)b2;

  bool b3 = (cex1 != cex2);
  (void)b3;

  auto alloc = execution::query(cex1, execution::allocator);
  (void)alloc;
}

template<class Executor>
void static_thread_pool_oneway_executor_compile_test(Executor ex1)
{
  static_thread_pool_executor_compile_test(ex1);

  static_assert(execution::is_oneway_executor_v<Executor>, "is_oneway_executor must evaluate true");

  const Executor& cex1 = ex1;

  cex1.execute([]{});

  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::blocking.never));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::blocking.possibly));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::blocking.always));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::relationship.fork));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::relationship.continuation));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::outstanding_work.untracked));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::outstanding_work.tracked));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::bulk_guarantee.parallel));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::mapping.thread));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::allocator));
  static_thread_pool_oneway_executor_compile_test(execution::require(cex1, execution::allocator(std::allocator<void>())));

  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::blocking.never));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::blocking.possibly));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::blocking.always));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::relationship.fork));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::relationship.continuation));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::outstanding_work.untracked));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::outstanding_work.tracked));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::bulk_guarantee.sequenced));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::bulk_guarantee.parallel));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::bulk_guarantee.unsequenced));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::mapping.thread));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::mapping.new_thread));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::allocator));
  static_thread_pool_oneway_executor_compile_test(execution::prefer(cex1, execution::allocator(std::allocator<void>())));
}

template<class Executor>
void static_thread_pool_bulk_oneway_executor_compile_test(Executor ex1)
{
  static_thread_pool_executor_compile_test(ex1);

  static_assert(execution::is_bulk_oneway_executor_v<Executor>, "is_bulk_oneway_executor must evaluate true");
  static_assert(std::is_same<execution::executor_shape_t<Executor>, std::size_t>::value, "shape type must be size_t");
  static_assert(std::is_same<execution::executor_index_t<Executor>, std::size_t>::value, "index type must be size_t");

  const Executor& cex1 = ex1;

  cex1.bulk_execute([](std::size_t, int&){}, 1, []{ return 42; });

  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::blocking.never));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::blocking.possibly));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::blocking.always));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::relationship.fork));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::relationship.continuation));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::outstanding_work.untracked));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::outstanding_work.tracked));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::bulk_guarantee.parallel));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::mapping.thread));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::allocator));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(cex1, execution::allocator(std::allocator<void>())));

  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::blocking.never));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::blocking.possibly));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::blocking.always));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::relationship.fork));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::relationship.continuation));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::outstanding_work.untracked));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::outstanding_work.tracked));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::bulk_guarantee.sequenced));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::bulk_guarantee.parallel));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::bulk_guarantee.unsequenced));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::mapping.thread));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::mapping.new_thread));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::allocator));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::prefer(cex1, execution::allocator(std::allocator<void>())));
}

void static_thread_pool_compile_test()
{
  using executor_type = static_thread_pool::executor_type;

  static_thread_pool pool1(0);
  static_thread_pool pool2(static_cast<std::size_t>(0));

  pool1.attach();

  pool1.stop();

  pool1.wait();

  executor_type ex1(pool1.executor());

  static_thread_pool_oneway_executor_compile_test(pool1.executor());
  static_thread_pool_oneway_executor_compile_test(execution::require(pool1.executor(), execution::oneway));
  static_thread_pool_oneway_executor_compile_test(execution::require(pool1.executor(), execution::bulk_oneway, execution::oneway));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(pool1.executor(), execution::bulk_oneway));
  static_thread_pool_bulk_oneway_executor_compile_test(execution::require(pool1.executor(), execution::bulk_oneway, execution::oneway, execution::bulk_oneway));
}

int main()
{
}
