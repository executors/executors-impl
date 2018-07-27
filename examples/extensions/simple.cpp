#include <experimental/execution>
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
    static constexpr bool is_requirable = false;
    static constexpr bool is_preferable = false;

    using polymorphic_query_result_type = std::function<std::shared_ptr<thing>(std::size_t)>;
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
  inline_executor_with_factory ex1;
  auto p1 = execution::query(ex1, custom_props::thing_factory)(1);
  p1->doit();

  using executor_t = execution::executor<
    execution::oneway_t,
    execution::single_t,
    custom_props::thing_factory_t
  >;

  executor_t ex2 = ex1;
  auto p2 = execution::query(ex2, custom_props::thing_factory)(2);
  p2->doit();
}
