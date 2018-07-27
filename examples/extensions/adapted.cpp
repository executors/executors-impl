#include <cassert>
#include <experimental/execution>
#include <experimental/type_traits>
#include <functional>
#include <iostream>
#include <memory>

namespace execution = std::experimental::execution;

namespace custom_props
{
  class thing
  {
  public:
    virtual ~thing() {}
    virtual void doit() = 0;
  };

  struct thing_factory_t
  {
    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = false;

    using polymorphic_query_result_type = std::function<std::shared_ptr<thing>(std::size_t)>;

    template <class InnerExecutor>
    class adapter
    {
      InnerExecutor inner_ex_;

    public:
      adapter(InnerExecutor ex) : inner_ex_(std::move(ex)) {}

      friend bool operator==(const adapter& a, const adapter& b) noexcept
      {
        return a.inner_ex_ == b.inner_ex_;
      }

      friend bool operator!=(const adapter& a, const adapter& b) noexcept
      {
        return !(a == b);
      }

      template <class Function,
          std::enable_if_t<
            std::is_same_v<Function, Function>
              && execution::single_t::static_query_v<InnerExecutor>
              && execution::oneway_t::static_query_v<InnerExecutor>
          >* = nullptr>
      void execute(Function f) const
      {
        inner_ex_.execute(std::move(f));
      }

      auto query(custom_props::thing_factory_t) const
      {
        return [](std::size_t n)
        {
          return std::make_shared<default_thing>(n);
        };
      }

      // Forward other kinds of query to the inner executor.

      template <class Executor, class Property>
      using static_member_query_t = decltype(Executor::query(std::declval<const Property&>()));

      template <class Executor, class Property>
      using member_query_t = decltype(std::declval<Executor>().query(std::declval<const Property&>()));

      template <class Property>
      static auto query(const Property& p,
           std::enable_if_t<
             std::experimental::is_detected_v<static_member_query_t, InnerExecutor, Property>
             && !std::experimental::is_detected_v<member_query_t, InnerExecutor, Property>
           >* = nullptr)
      {
        return InnerExecutor::query(p);
      };

      template <class Property>
      auto query(const Property& p,
           std::enable_if_t<
             !std::experimental::is_detected_v<static_member_query_t, InnerExecutor, Property>
             && std::experimental::is_detected_v<member_query_t, InnerExecutor, Property>
           >* = nullptr) const
      {
        return inner_ex_.query(p);
      };
    };

    template <class Executor,
         std::enable_if_t<
           !execution::can_query_v<Executor, thing_factory_t>
         >* = nullptr>
    friend adapter<Executor> require(Executor ex, thing_factory_t)
    {
      return adapter<Executor>(std::move(ex));
    }

    template <class Executor,
         std::enable_if_t<
           execution::can_query_v<Executor, thing_factory_t>
         >* = nullptr>
    friend Executor require(Executor ex, thing_factory_t)
    {
      return ex;
    }

  private:
    class default_thing : public custom_props::thing
    {
    public:
      default_thing(std::size_t n)
        : n_(n)
      {
      }

      void doit() override
      {
        std::cout << "Default thing " << n_ << "\n";
      }

    private:
      std::size_t n_;
    };
  };

  inline constexpr thing_factory_t thing_factory;
} // namespace custom_props

class inline_executor
{
public:
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
    f();
  }
};

static_assert(execution::is_oneway_executor_v<inline_executor>, "one way executor requirements not met");
static_assert(!execution::can_query_v<inline_executor, custom_props::thing_factory_t>, "thing_factory property present");

class inline_executor_with_factory
{
public:
  friend bool operator==(const inline_executor_with_factory&, const inline_executor_with_factory&) noexcept
  {
    return true;
  }

  friend bool operator!=(const inline_executor_with_factory&, const inline_executor_with_factory&) noexcept
  {
    return false;
  }

  template <class Function>
  void execute(Function f) const noexcept
  {
    f();
  }

  auto query(custom_props::thing_factory_t) const
  {
    return [](std::size_t n)
    {
      return std::make_shared<concrete_thing>(n);
    };
  }

private:
  class concrete_thing : public custom_props::thing
  {
  public:
    concrete_thing(std::size_t n)
      : n_(n)
    {
    }

    void doit() override
    {
      std::cout << "Thing " << n_ << "\n";
    }

  private:
    std::size_t n_;
  };
};

static_assert(execution::is_oneway_executor_v<inline_executor_with_factory>, "one way executor requirements not met");
static_assert(execution::can_query_v<inline_executor_with_factory, custom_props::thing_factory_t>, "thing_factory property missing");

int main()
{
  inline_executor ex1;
  auto ex2 = execution::require(ex1, custom_props::thing_factory);
  auto p1 = execution::query(ex2, custom_props::thing_factory)(1);
  p1->doit();

  inline_executor_with_factory ex3;
  auto p2 = execution::query(ex3, custom_props::thing_factory)(2);
  p2->doit();

  using executor_t = execution::executor<
    execution::oneway_t,
    execution::single_t,
    custom_props::thing_factory_t
  >;

  executor_t ex4 = ex1;
  auto f1 = execution::query(ex4, custom_props::thing_factory);
  assert(f1 == nullptr);

  executor_t ex5 = execution::require(ex4, custom_props::thing_factory);
  auto p3 = execution::query(ex5, custom_props::thing_factory)(3);
  p3->doit();

  executor_t ex6 = ex2;
  auto p4 = execution::query(ex6, custom_props::thing_factory)(4);
  p4->doit();

  executor_t ex7 = ex3;
  auto p5 = execution::query(ex7, custom_props::thing_factory)(5);
  p5->doit();
}
