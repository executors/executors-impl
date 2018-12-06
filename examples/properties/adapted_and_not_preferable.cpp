#include <thread_pool>
#include <cassert>
#include <iostream>

namespace execution = std::execution;
using std::static_thread_pool;

namespace custom_props
{
  struct tracing
  {
    template<class Executor>
      static constexpr bool is_applicable_v =
        std::execution::is_oneway_executor_v<Executor>;

    static constexpr bool is_requirable_concept = false;
    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = false;
    using polymorphic_query_result_type = bool;

    bool on = false;
  };

  // Requiring the property defaults to an adapter.

  template <class InnerExecutor>
  class tracing_executor
  {
    bool tracing_;
    InnerExecutor inner_ex_;

    template <class T> static auto inner_declval() -> decltype(std::declval<InnerExecutor>());

  public:
    tracing_executor(bool on, const InnerExecutor& ex)
      : tracing_(on), inner_ex_(ex) {}

    // Intercept require requests for tracing.
    tracing_executor require(custom_props::tracing t) const { return { t.on, inner_ex_ }; }

    // Forward other kinds of require to the inner executor.
    template <class Property> auto require(const Property& p) const &
      -> tracing_executor<decltype(inner_declval<Property>().require(p))>
        { return { tracing_, inner_ex_.require(p) }; }
    template <class Property> auto require(const Property& p) &&
      -> tracing_executor<decltype(inner_declval<Property>().require(p))>
        { return { tracing_, std::move(inner_ex_).require(p) }; }

    // Intercept query requests for tracing.
    bool query(custom_props::tracing) const { return tracing_; }

    // Forward other kinds of query to the inner executor.
    template<class Property> auto query(const Property& p) const
      -> decltype(inner_declval<Property>().query(p))
        { return inner_ex_.query(p); }

    friend bool operator==(const tracing_executor& a, const tracing_executor& b) noexcept
    {
      return a.tracing_ == b.tracing_ && a.inner_ex_ == b.inner_ex_;
    }

    friend bool operator!=(const tracing_executor& a, const tracing_executor& b) noexcept
    {
      return !(a == b);
    }

    template <class Function>
    auto execute(Function f) const
      -> decltype(inner_declval<Function>().execute(std::move(f)))
    {
      return inner_ex_.execute(
          [tracing = tracing_, f = std::move(f)]() mutable
          {
            if (tracing) std::cout << "running function adapted\n";
            return f();
          });
    }
  };

  template <class Executor>
    tracing_executor<Executor> require(Executor ex, tracing t)
      { return { t.on, std::move(ex) }; }
};

class inline_executor
{
public:
  inline_executor require(custom_props::tracing t) const { inline_executor tmp(*this); tmp.tracing_ = t.on; return tmp; }

  bool query(custom_props::tracing) const { return tracing_; }

  friend bool operator==(const inline_executor&, const inline_executor&) noexcept
  {
    return true;
  }

  friend bool operator!=(const inline_executor&, const inline_executor&) noexcept
  {
    return false;
  }

  template <class Function>
  void execute(Function f) const noexcept
  {
    if (tracing_) std::cout << "running function inline\n";
    f();
  }

private:
  bool tracing_;
};

static_assert(execution::is_oneway_executor_v<inline_executor>, "one way executor requirements not met");
static_assert(execution::is_oneway_executor_v<custom_props::tracing_executor<static_thread_pool::executor_type>>, "one way executor requirements not met");

int main()
{
  static_thread_pool pool{1};

  auto ex1 = std::require(inline_executor(), custom_props::tracing{true});
  assert(std::query(ex1, custom_props::tracing{}));
  ex1.execute([]{ std::cout << "we made it\n"; });

  static_assert(!std::can_prefer_v<inline_executor, custom_props::tracing>, "cannot prefer");

  auto ex3 = std::require(pool.executor(), custom_props::tracing{true});
  assert(std::query(ex3, custom_props::tracing{}));
  ex3.execute([]{ std::cout << "we made it again\n"; });

  static_assert(!std::can_prefer_v<static_thread_pool::executor_type, custom_props::tracing>, "cannot prefer");

  execution::executor<execution::oneway_t, custom_props::tracing> ex5 = pool.executor();
  auto ex6 = std::require(ex5, custom_props::tracing{true});
  assert(std::query(ex6, custom_props::tracing{}));
  ex6.execute([]{ std::cout << "and again\n"; });

  static_assert(!std::can_prefer_v<decltype(ex5), custom_props::tracing>, "cannot prefer");

  pool.wait();
}
