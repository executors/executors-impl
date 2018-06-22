#ifndef STD_EXPERIMENTAL_BITS_DIRECTIONALITY_H
#define STD_EXPERIMENTAL_BITS_DIRECTIONALITY_H

#include <experimental/bits/adapter.h>
#include <experimental/bits/blocking_adaptation.h>
#include <experimental/bits/is_oneway_executor.h>
#include <experimental/bits/is_twoway_executor.h>
#include <experimental/bits/is_bulk_oneway_executor.h>
#include <experimental/bits/is_bulk_twoway_executor.h>
#include <future>
#include <tuple>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

struct oneway_t
{
  static constexpr bool is_requirable = true;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  template<class Executor>
    static constexpr bool static_query_v
      = is_oneway_executor<Executor>::value
        || is_bulk_oneway_executor<Executor>::value;

  static constexpr bool value() { return true; }
};

constexpr oneway_t oneway;

struct twoway_t
{
  static constexpr bool is_requirable = true;
  static constexpr bool is_preferable = false;

  using polymorphic_query_result_type = bool;

  template<class Executor>
    static constexpr bool static_query_v
      = is_twoway_executor<Executor>::value
        || is_bulk_twoway_executor<Executor>::value;

  static constexpr bool value() { return true; }

private:
  template<class Executor>
  class adapter : public impl::adapter<adapter, Executor>
  {
    template <class T> static auto inner_declval() -> decltype(std::declval<Executor>());
    template <class, class T> struct dependent_type { using type = T; };

  public:
    using impl::adapter<adapter, Executor>::adapter;

    template<class Function> auto execute(Function f) const
      -> decltype(inner_declval<Function>().execute(std::move(f)))
    {
      this->executor_.execute(std::move(f));
    }

    template<class Function>
    auto twoway_execute(Function f) const -> typename dependent_type<
        decltype(inner_declval<Function>().execute(std::move(f))),
        future<decltype(f())>>::type
    {
      packaged_task<decltype(f())()> task(std::move(f));
      future<decltype(f())> future = task.get_future();
      this->executor_.execute(std::move(task));
      return future;
    }

    template<class Function, class SharedFactory>
    auto bulk_execute(Function f, std::size_t n, SharedFactory sf) const
      -> decltype(inner_declval<Function>().bulk_execute(std::move(f), n, std::move(sf)))
    {
      this->executor_.bulk_execute(std::move(f), n, std::move(sf));
    }

    template<class Function, class ResultFactory, class SharedFactory>
    auto bulk_twoway_execute(Function f, std::size_t n, ResultFactory rf, SharedFactory sf) const -> typename dependent_type<
        decltype(inner_declval<Function>().bulk_execute(std::move(f), n, std::move(sf))),
        typename std::enable_if<is_same<decltype(rf()), void>::value, future<void>>::type>::type
    {
      auto shared_state = std::make_shared<
          std::tuple<
            std::atomic<std::size_t>, // Number of incomplete functions.
            decltype(sf()), // Underlying shared state.
            std::atomic<std::size_t>, // Number of exceptions raised.
            std::exception_ptr, // First exception raised.
            promise<void> // Promise to receive result
          >>(n, sf(), 0, nullptr, promise<void>());
      future<void> future = std::get<4>(*shared_state).get_future();
      this->executor_.bulk_execute(
          [f = std::move(f)](auto i, auto& s) mutable
          {
            try
            {
              f(i, std::get<1>(*s));
            }
            catch (...)
            {
              if (std::get<2>(*s)++ == 0)
                std::get<3>(*s) = std::current_exception();
            }
            if (--std::get<0>(*s) == 0)
            {
              if (std::get<3>(*s))
                std::get<4>(*s).set_exception(std::get<3>(*s));
              else
                std::get<4>(*s).set_value();
            }
          }, n, [shared_state]{ return shared_state; });
      return future;
    }

    template<class Function, class ResultFactory, class SharedFactory>
    auto bulk_twoway_execute(Function f, std::size_t n, ResultFactory rf, SharedFactory sf) const -> typename dependent_type<
        decltype(inner_declval<Function>().bulk_execute(std::move(f), n, std::move(sf))),
        typename std::enable_if<!is_same<decltype(rf()), void>::value, future<decltype(rf())>>::type>::type
    {
      auto shared_state = std::make_shared<
          std::tuple<
            std::atomic<std::size_t>, // Number of incomplete functions.
            decltype(rf()), // Result.
            decltype(sf()), // Underlying shared state.
            std::atomic<std::size_t>, // Number of exceptions raised.
            std::exception_ptr, // First exception raised.
            promise<decltype(rf())> // Promise to receive result
          >>(n, rf(), sf(), 0, nullptr, promise<decltype(rf())>());
      future<decltype(rf())> future = std::get<5>(*shared_state).get_future();
      this->executor_.bulk_execute(
          [f = std::move(f)](auto i, auto& s) mutable
          {
            try
            {
              f(i, std::get<1>(*s), std::get<2>(*s));
            }
            catch (...)
            {
              if (std::get<3>(*s)++ == 0)
                std::get<4>(*s) = std::current_exception();
            }
            if (--std::get<0>(*s) == 0)
            {
              if (std::get<4>(*s))
                std::get<5>(*s).set_exception(std::get<4>(*s));
              else
                std::get<5>(*s).set_value(std::move(std::get<1>(*s)));
            }
          }, n, [shared_state]{ return shared_state; });
      return future;
    }
  };
public:
  // Default require for twoway adapts oneway executors.
  template <typename Executor, typename =
      std::enable_if_t<
        (is_oneway_executor<Executor>::value || is_bulk_oneway_executor<Executor>::value)
          && !(is_twoway_executor<Executor>::value || is_bulk_twoway_executor<Executor>::value)
          && blocking_adaptation_t::static_query_v<Executor> == blocking_adaptation.allowed
      >>
  friend adapter<Executor> require(Executor ex, twoway_t)
  {
    return adapter<Executor>(std::move(ex));
  }
};

constexpr twoway_t twoway;

} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_DIRECTIONALITY_H
