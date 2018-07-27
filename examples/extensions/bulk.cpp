#include <experimental/execution>
#include <experimental/type_traits>
#include <cassert>
#include <functional>
#include <iostream>

namespace execution = std::experimental::execution;

namespace custom_props
{
  struct bulk_execute_t
  {
    static constexpr bool is_requirable = false;
    static constexpr bool is_preferable = false;

    class polymorphic_query_result_type
    {
    public:
      template <class Target>
      polymorphic_query_result_type(Target t)
        : target_(std::make_shared<target<Target>>(std::move(t)))
      {
      }

      template <class Function, class SharedFactory>
      void operator()(Function f, std::size_t n, SharedFactory sf) const
      {
        target_->call(std::make_unique<function<Function, SharedFactory>>(std::move(f)),
            n, std::make_shared<shared_factory<SharedFactory>>(std::move(sf)));
      }

    private:
      struct function_base
      {
        virtual ~function_base() {};
        virtual void call(std::size_t, std::shared_ptr<void>) = 0;
      };

      template <class Function, class SharedFactory>
      struct function : function_base
      {
        Function function_;

        function(Function f) : function_(std::move(f)) {}

        void call(std::size_t i, std::shared_ptr<void> ss) override
        {
          using shared_t = decltype(std::declval<SharedFactory>()());
          function_(i, *std::static_pointer_cast<shared_t>(ss));
        }
      };

      struct shared_factory_base
      {
        virtual ~shared_factory_base() {}
        virtual std::shared_ptr<void> call() = 0;
      };

      template <class SharedFactory>
      struct shared_factory : shared_factory_base
      {
        SharedFactory factory_;

        shared_factory(SharedFactory sf) : factory_(std::move(sf)) {}

        std::shared_ptr<void> call() override
        {
          SharedFactory factory(std::move(factory_));
          using shared_t = decltype(factory());
          return std::make_shared<shared_t>(factory());
        }
      };

      struct target_base
      {
        virtual ~target_base() {}
        virtual void call(std::unique_ptr<function_base>, std::size_t, std::shared_ptr<shared_factory_base>) = 0;
      };

      template <class Target>
      struct target : target_base
      {
        Target target_;

        target(Target t) : target_(std::move(t)) {}

        void call(std::unique_ptr<function_base> f, std::size_t n, std::shared_ptr<shared_factory_base> sf) override
        {
          target_(
              [f = std::move(f)](std::size_t i, auto s) mutable { f->call(i, s); },
              n, [sf = std::move(sf)]() mutable { return sf->call(); });
        }
      };

      std::shared_ptr<target_base> target_;
    };
  };

  inline constexpr bulk_execute_t bulk_execute;

  struct bulk_t
  {
    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = false;
    using polymorphic_query_result_type = bool;

    template <class Executor>
    static constexpr bool static_query_v =
      execution::can_query_v<Executor, bulk_execute_t>;

    static constexpr bool value() { return true; }

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

      // Assumes this function is required of all executors as a basic building block.
      template <class Function>
      void execute(Function f) const
      {
        return inner_ex_.execute(std::move(f));
      }

      auto query(custom_props::bulk_execute_t) const
      {
        return [ex = inner_ex_](auto f, std::size_t n, auto sf)
        {
          // A naive default adaptation.
          ex.execute(
              [f = std::move(f), n, sf = std::move(sf)]() mutable
              {
                auto ss{sf()};
                for (std::size_t i = 0; i < n; ++i)
                  f(i, ss);
              });
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
           !execution::can_query_v<Executor, bulk_execute_t>
         >* = nullptr>
    friend adapter<Executor> require(Executor ex, bulk_t)
    {
      return adapter<Executor>(std::move(ex));
    }
  };

  inline constexpr bulk_t bulk;
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
static_assert(!execution::can_query_v<inline_executor, custom_props::bulk_execute_t>, "bulk_execute property incorrect");

class inline_executor_with_bulk
{
public:
  friend bool operator==(const inline_executor_with_bulk&, const inline_executor_with_bulk&) noexcept
  {
    return true;
  }

  friend bool operator!=(const inline_executor_with_bulk&, const inline_executor_with_bulk&) noexcept
  {
    return false;
  }

  template <class Function>
  void execute(Function f) const noexcept
  {
    f();
  }

  auto query(custom_props::bulk_execute_t) const
  {
    return [](auto f, std::size_t n, auto sf)
    {
      auto ss{sf()};
      for (std::size_t i = 0; i < n; ++i)
        f(i, ss);
    };
  }
};

static_assert(execution::is_oneway_executor_v<inline_executor_with_bulk>, "one way executor requirements not met");
static_assert(execution::can_query_v<inline_executor_with_bulk, custom_props::bulk_execute_t>, "bulk_execute property incorrect");

int main()
{
  inline_executor ex1;
  assert(!execution::query(ex1, custom_props::bulk));

  auto ex2 = execution::require(ex1, custom_props::bulk);
  assert(execution::query(ex2, custom_props::bulk));
  execution::query(ex2, custom_props::bulk_execute)(
      [](std::size_t i, auto ss){ std::cout << i << ": " << ss << "\n"; },
      10, []{ return std::string("adapted"); });

  inline_executor_with_bulk ex3;
  assert(execution::query(ex3, custom_props::bulk));
  execution::query(ex3, custom_props::bulk_execute)(
      [](std::size_t i, auto ss){ std::cout << i << ": " << ss << "\n"; },
      10, []{ return std::string("native"); });

  auto ex4 = execution::require(ex3, custom_props::bulk);
  assert(execution::query(ex4, custom_props::bulk));
  execution::query(ex4, custom_props::bulk_execute)(
      [](std::size_t i, auto ss){ std::cout << i << ": " << ss << "\n"; },
      10, []{ return std::string("required native"); });

  using executor_t = execution::executor<
    // These two properties would not be here if plain execute becomes a
    // requirement on all executors, as a fundamental building block.
    execution::oneway_t, execution::single_t,
    // The following property is the only one that would be present.
    custom_props::bulk_execute_t
  >;

  executor_t ex5 = ex2;
  assert(execution::query(ex5, custom_props::bulk));
  execution::query(ex5, custom_props::bulk_execute)(
      [](std::size_t i, auto ss){ std::cout << i << ": " << ss << "\n"; },
      10, []{ return std::string("polymorphic adapted"); });

  executor_t ex6 = ex3;
  assert(execution::query(ex6, custom_props::bulk));
  execution::query(ex6, custom_props::bulk_execute)(
      [](std::size_t i, auto ss){ std::cout << i << ": " << ss << "\n"; },
      10, []{ return std::string("polymorphic native"); });

  executor_t ex7 = execution::require(ex6, custom_props::bulk);
  assert(execution::query(ex7, custom_props::bulk));
  execution::query(ex7, custom_props::bulk_execute)(
      [](std::size_t i, auto ss){ std::cout << i << ": " << ss << "\n"; },
      10, []{ return std::string("required polymorphic native"); });
}
