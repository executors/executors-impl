#include <experimental/execution>
#include <experimental/thread_pool>

namespace execution = std::experimental::execution;
using std::experimental::static_thread_pool;

using oneway_executor = execution::executor<
    execution::oneway_t,
    execution::bulk_oneway_t,
    execution::mapping_t::thread_t,
    execution::blocking_t::never_t,
    execution::blocking_t::possibly_t,
    execution::blocking_t::always_t
  >;

using bulk_oneway_executor = execution::executor<
    execution::bulk_oneway_t,
    execution::oneway_t,
    execution::mapping_t::thread_t,
    execution::blocking_t::never_t,
    execution::blocking_t::possibly_t,
    execution::blocking_t::always_t
  >;

void oneway_executor_compile_test()
{
  static_assert(execution::is_oneway_executor_v<oneway_executor>, "is_oneway_executor must evaluate true");

  static_thread_pool pool(0);

  static_assert(noexcept(oneway_executor()), "default constructor must not throw");
  static_assert(noexcept(oneway_executor(nullptr)), "nullptr constructor must not throw");

  oneway_executor ex1;
  oneway_executor ex2(nullptr);

  const oneway_executor& cex1 = ex1;
  const oneway_executor& cex2 = ex2;

  static_assert(noexcept(oneway_executor(cex1)), "copy constructor must not throw");
  static_assert(noexcept(oneway_executor(std::move(ex1))), "move constructor must not throw");

  oneway_executor ex3(ex1);
  oneway_executor ex4(std::move(ex1));

  oneway_executor ex5(pool.executor());

  execution::executor<execution::oneway_t> ex6(ex5);
  execution::executor<execution::oneway_t, execution::bulk_oneway_t> ex7(ex5);

  static_assert(noexcept(ex2 = cex1), "copy assignment must not throw");
  static_assert(noexcept(ex3 = std::move(ex1)), "move assignment must not throw");
  static_assert(noexcept(ex3 = nullptr), "nullptr assignment must not throw");

  ex2 = ex1;
  ex3 = std::move(ex1);
  ex4 = nullptr;
  ex5 = pool.executor();

  static_assert(noexcept(ex1.swap(ex2)), "swap must not throw");

  ex1.swap(ex2);

  ex1.assign(pool.executor());

  bulk_oneway_executor ex8 = execution::require(cex1, execution::bulk_oneway);

  ex1 = execution::require(cex1, execution::oneway);
  ex1 = execution::require(cex1, execution::mapping.thread);
  ex1 = execution::require(cex1, execution::blocking.never);
  ex1 = execution::require(cex1, execution::blocking.possibly);
  ex1 = execution::require(cex1, execution::blocking.always);

  ex1 = execution::prefer(cex1, execution::mapping.thread);
  ex1 = execution::prefer(cex1, execution::blocking.never);
  ex1 = execution::prefer(cex1, execution::blocking.possibly);
  ex1 = execution::prefer(cex1, execution::blocking.always);
  ex1 = execution::prefer(cex1, execution::relationship.fork);
  ex1 = execution::prefer(cex1, execution::relationship.continuation);
  ex1 = execution::prefer(cex1, execution::outstanding_work.untracked);
  ex1 = execution::prefer(cex1, execution::outstanding_work.tracked);
  ex1 = execution::prefer(cex1, execution::bulk_guarantee.sequenced);
  ex1 = execution::prefer(cex1, execution::bulk_guarantee.parallel);
  ex1 = execution::prefer(cex1, execution::bulk_guarantee.unsequenced);
  ex1 = execution::prefer(cex1, execution::mapping.new_thread);

  cex1.execute([]{});

  //cex1.bulk_execute([](std::size_t, int&){}, 1, []{ return 42; });

  bool b1 = static_cast<bool>(ex1);
  (void)b1;

  const std::type_info& target_type = cex1.target_type();
  (void)target_type;

  static_thread_pool::executor_type* ex9 = ex1.target<static_thread_pool::executor_type>();
  (void)ex9;

  const static_thread_pool::executor_type* cex6 = ex1.target<static_thread_pool::executor_type>();
  (void)cex6;

  bool b2 = (cex1 == cex2);
  (void)b2;

  bool b3 = (cex1 != cex2);
  (void)b3;

  bool b4 = (cex1 == nullptr);
  (void)b4;

  bool b5 = (cex1 != nullptr);
  (void)b5;

  bool b6 = (nullptr == cex2);
  (void)b6;

  bool b7 = (nullptr != cex2);
  (void)b7;

  swap(ex1, ex2);
}

void bulk_oneway_executor_compile_test()
{
  static_assert(execution::is_bulk_oneway_executor_v<bulk_oneway_executor>, "is_bulk_oneway_executor must evaluate true");

  static_thread_pool pool(0);

  static_assert(noexcept(bulk_oneway_executor()), "default constructor must not throw");
  static_assert(noexcept(bulk_oneway_executor(nullptr)), "nullptr constructor must not throw");

  bulk_oneway_executor ex1;
  bulk_oneway_executor ex2(nullptr);

  const bulk_oneway_executor& cex1 = ex1;
  const bulk_oneway_executor& cex2 = ex2;

  static_assert(noexcept(bulk_oneway_executor(cex1)), "copy constructor must not throw");
  static_assert(noexcept(bulk_oneway_executor(std::move(ex1))), "move constructor must not throw");

  bulk_oneway_executor ex3(ex1);
  bulk_oneway_executor ex4(std::move(ex1));

  bulk_oneway_executor ex5(pool.executor());

  execution::executor<execution::oneway_t> ex6(ex5);
  execution::executor<execution::oneway_t, execution::bulk_oneway_t> ex7(ex5);

  static_assert(noexcept(ex2 = cex1), "copy assignment must not throw");
  static_assert(noexcept(ex3 = std::move(ex1)), "move assignment must not throw");
  static_assert(noexcept(ex3 = nullptr), "nullptr assignment must not throw");

  ex2 = ex1;
  ex3 = std::move(ex1);
  ex4 = nullptr;
  ex5 = pool.executor();

  static_assert(noexcept(ex1.swap(ex2)), "swap must not throw");

  ex1.swap(ex2);

  ex1.assign(pool.executor());

  oneway_executor ex8 = execution::require(cex1, execution::oneway);

  ex1 = execution::require(cex1, execution::bulk_oneway);
  ex1 = execution::require(cex1, execution::mapping.thread);
  ex1 = execution::require(cex1, execution::blocking.never);
  ex1 = execution::require(cex1, execution::blocking.possibly);
  ex1 = execution::require(cex1, execution::blocking.always);

  ex1 = execution::prefer(cex1, execution::mapping.thread);
  ex1 = execution::prefer(cex1, execution::blocking.never);
  ex1 = execution::prefer(cex1, execution::blocking.possibly);
  ex1 = execution::prefer(cex1, execution::blocking.always);
  ex1 = execution::prefer(cex1, execution::relationship.fork);
  ex1 = execution::prefer(cex1, execution::relationship.continuation);
  ex1 = execution::prefer(cex1, execution::outstanding_work.untracked);
  ex1 = execution::prefer(cex1, execution::outstanding_work.tracked);
  ex1 = execution::prefer(cex1, execution::bulk_guarantee.sequenced);
  ex1 = execution::prefer(cex1, execution::bulk_guarantee.parallel);
  ex1 = execution::prefer(cex1, execution::bulk_guarantee.unsequenced);
  ex1 = execution::prefer(cex1, execution::mapping.new_thread);

  cex1.execute([](std::size_t, int&){}, 1, []{ return 42; });

  bool b1 = static_cast<bool>(ex1);
  (void)b1;

  const std::type_info& target_type = cex1.target_type();
  (void)target_type;

  static_thread_pool::executor_type* ex9 = ex1.target<static_thread_pool::executor_type>();
  (void)ex9;

  const static_thread_pool::executor_type* cex6 = ex1.target<static_thread_pool::executor_type>();
  (void)cex6;

  bool b2 = (cex1 == cex2);
  (void)b2;

  bool b3 = (cex1 != cex2);
  (void)b3;

  bool b4 = (cex1 == nullptr);
  (void)b4;

  bool b5 = (cex1 != nullptr);
  (void)b5;

  bool b6 = (nullptr == cex2);
  (void)b6;

  bool b7 = (nullptr != cex2);
  (void)b7;

  swap(ex1, ex2);
}

int main()
{
}
