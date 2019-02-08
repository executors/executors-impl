#include <iostream>
#include "for_each_n.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>


namespace execution = std::execution;


// creating EAs with this executor always fails via exception
class throwing_executor
{
  public:
    constexpr static auto query(execution::executor_concept_t)
    {
      return execution::bulk_oneway;
    }

    friend bool operator==(const throwing_executor&, const throwing_executor&) noexcept
    {
      return true;
    }

    friend bool operator!=(const throwing_executor&, const throwing_executor&) noexcept
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

    template<class Function, class Factory>
    void bulk_execute(Function f, size_t n, Factory shared_factory) const
    {
      throw 13;
    }
};


int main()
{
  {
    // test with throwing_executor, vector

    std::vector<int> vec(10);

    std::static_thread_pool pool(4);

    for_each_n(par.on(throwing_executor()), vec.begin(), vec.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));
  }

  {
    // test with throwing_executor, list

    std::list<int> lst(10);

    std::static_thread_pool pool(4);

    for_each_n(par.on(throwing_executor()), lst.begin(), lst.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(lst.begin(), lst.end(), 42) == static_cast<int>(lst.size()));
  }

  std::cout << "OK" << std::endl;

  return 0;
}

