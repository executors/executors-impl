#include <iostream>
#include "for_each_n.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>


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
    // test with bulk_inline_executor, vector

    std::vector<int> vec(10);

    std::experimental::static_thread_pool pool(4);

    for_each_n(par.on(bulk_inline_executor()), vec.begin(), vec.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));
  }

  {
    // test with bulk_inline_executor, list

    std::list<int> lst(10);

    std::experimental::static_thread_pool pool(4);

    for_each_n(par.on(bulk_inline_executor()), lst.begin(), lst.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(lst.begin(), lst.end(), 42) == static_cast<int>(lst.size()));
  }

  // XXX the following tests correctly call std::terminate upon 
  //     exception in an element access function
  //     they are disabled to allow the test program to complete normally

  //{
  //  // test with bulk_inline_executor, vector, throwing function

  //  std::vector<int> vec(10);

  //  for_each_n(par.on(bulk_inline_executor()), vec.begin(), vec.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with bulk_inline_executor, list, throwing function

  //  std::list<int> lst(10);

  //  for_each_n(par.on(bulk_inline_executor()), lst.begin(), lst.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with bulk_inline_executor, vector, throwing iterator

  //  std::vector<int> vec(10);

  //  for_each_n(par.on(bulk_inline_executor()), make_throwing_iterator(vec.begin()), vec.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with bulk_inline_executor, list, throwing iterator

  //  std::list<int> lst(10);

  //  for_each_n(par.on(bulk_inline_executor()), make_throwing_iterator(lst.begin()), lst.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  std::cout << "OK" << std::endl;

  return 0;
}

