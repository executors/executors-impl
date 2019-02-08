#include <iostream>
#include "for_each_n.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>


namespace execution = std::execution;


class inline_executor
{
  public:
    constexpr static auto query(execution::executor_concept_t)
    {
      return execution::oneway;
    }

    friend bool operator==(const inline_executor&, const inline_executor&) noexcept
    {
      return true;
    }

    friend bool operator!=(const inline_executor&, const inline_executor&) noexcept
    {
      return false;
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

    template<class Function>
    void execute(Function f) const noexcept
    {
      f();
    }
};


int main()
{
  {
    // test with inline_executor, vector

    std::vector<int> vec(10);

    for_each_n(par.on(inline_executor()), vec.begin(), vec.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));
  }

  {
    // test with inline_executor, list

    std::list<int> lst(10);

    for_each_n(par.on(inline_executor()), lst.begin(), lst.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(lst.begin(), lst.end(), 42) == static_cast<int>(lst.size()));
  }

  // XXX the following tests correctly call std::terminate upon 
  //     exception in an element access function
  //     they are disabled to allow the test program to complete normally

  //{
  //  // test with inline_executor, vector, throwing function

  //  std::vector<int> vec(10);

  //  for_each_n(par.on(inline_executor()), vec.begin(), vec.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with inline_executor, list, throwing function

  //  std::list<int> lst(10);

  //  for_each_n(par.on(inline_executor()), lst.begin(), lst.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with inline_executor, vector, throwing iterator

  //  std::vector<int> vec(10);

  //  for_each_n(par.on(inline_executor()), make_throwing_iterator(vec.begin()), vec.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with inline_executor, list, throwing iterator

  //  std::list<int> lst(10);

  //  for_each_n(par.on(inline_executor()), make_throwing_iterator(lst.begin()), lst.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  std::cout << "OK" << std::endl;

  return 0;
}

