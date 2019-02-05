#include "reduce.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <experimental/thread_pool>
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <functional>
#include <numeric>


namespace execution = std::experimental::execution;


class bulk_inline_executor
{
  public:
    friend bool operator==(const bulk_inline_executor&, const bulk_inline_executor&) noexcept
    {
      return true;
    }

    friend bool operator!=(const bulk_inline_executor&, const bulk_inline_executor&) noexcept
    {
      return false;
    }

    bulk_inline_executor require(execution::oneway_t) const
    {
      return *this;
    }

    bulk_inline_executor require(execution::bulk_t) const
    {
      return *this;
    }

    constexpr static execution::bulk_guarantee_t::parallel_t query(execution::bulk_guarantee_t)
    {
      return execution::bulk_guarantee.parallel;
    }

    constexpr static execution::blocking_t::always_t query(execution::blocking_t)
    {
      return execution::blocking.always;
    }

    constexpr static execution::mapping_t::thread_t query(execution::mapping_t)
    {
      return execution::mapping.thread;
    }

    template<class Function, class Factory>
    void bulk_execute(Function f, size_t n, Factory shared_factory) const noexcept
    {
      auto shared = shared_factory();

      for(size_t i = 0; i < n; ++i)
      {
        f(i, shared);
      }
    }
};

int main()
{
  {
    // test with par, vector

    std::vector<int> vec(10);
    std::iota(vec.begin(), vec.end(), 1);

    int result = reduce(par.on(bulk_inline_executor()), vec.begin(), vec.end(), 13, std::plus<>());

    assert(std::accumulate(vec.begin(), vec.end(), 13) == result);
  }

  {
    // test with par, list

    std::list<int> lst(10);
    std::iota(lst.begin(), lst.end(), 1);

    int result = reduce(par.on(bulk_inline_executor()), lst.begin(), lst.end(), 13, std::plus<>());

    assert(std::accumulate(lst.begin(), lst.end(), 13) == result);
  }

  // XXX the following tests correctly call std::terminate upon 
  //     exception in an element access function
  //     they are disabled to allow the test program to complete normally

  //{
  //  // test with par, vector, throwing function

  //  std::vector<int> vec(10);
  //  std::iota(vec.begin(), vec.end(), 1);

  //  try
  //  {
  //    reduce(par.on(bulk_inline_executor()), vec.begin(), vec.end(), 13, [](int x, int y)
  //    {
  //      throw 13;

  //      return x + y;
  //    });
  //  }
  //  catch(...) {}

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, list, throwing function

  //  std::list<int> lst(10);
  //  std::iota(lst.begin(), lst.end(), 1);

  //  try
  //  {
  //    reduce(par.on(bulk_inline_executor()), lst.begin(), lst.end(), 13, [](int x, int y)
  //    {
  //      throw 13;

  //      return x + y;
  //    });
  //  }
  //  catch(...) {}

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, vector, throwing iterator

  //  std::vector<int> vec(10);
  //  std::iota(vec.begin(), vec.end(), 1);

  //  try
  //  {
  //    reduce(par.on(bulk_inline_executor()), make_throwing_iterator(vec.begin()), make_throwing_iterator(vec.end()), 13, std::plus<>());
  //  }
  //  catch(...) {}

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, list, throwing iterator

  //  std::list<int> lst(10);
  //  std::iota(lst.begin(), lst.end(), 1);

  //  try
  //  {
  //    reduce(par.on(bulk_inline_executor()), make_throwing_iterator(lst.begin()), make_throwing_iterator(lst.end()), 13, std::plus<>());
  //  }
  //  catch(...) {}

  //  // this line should never be reached
  //  assert(false);
  //}

  std::cout << "OK" << std::endl;

  return 0;
}


