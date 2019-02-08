#include "for_each_n.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <thread_pool>
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>
#include <iostream>


int main()
{
  {
    // test with static_thread_pool, vector

    std::vector<int> vec(10);

    std::static_thread_pool pool(4);

    for_each_n(par.on(pool.executor()), vec.begin(), vec.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));
  }

  {
    // test with static_thread_pool, list

    std::list<int> lst(10);

    std::static_thread_pool pool(4);

    for_each_n(par.on(pool.executor()), lst.begin(), lst.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(lst.begin(), lst.end(), 42) == static_cast<int>(lst.size()));
  }

  // XXX the following tests correctly call std::terminate upon 
  //     exception in an element access function
  //     they are disabled to allow the test program to complete normally

  //{
  //  // test with static_thread_pool, vector, throwing function

  //  std::vector<int> vec(10);

  //  std::static_thread_pool pool(4);

  //  for_each_n(par.on(pool.executor()), vec.begin(), vec.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with static_thread_pool, list, throwing function

  //  std::list<int> lst(10);

  //  std::static_thread_pool pool(4);

  //  for_each_n(par.on(pool.executor()), lst.begin(), lst.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with static_thread_pool, vector, throwing iterator

  //  std::vector<int> vec(10);

  //  std::static_thread_pool pool(4);

  //  for_each_n(par.on(pool.executor()), make_throwing_iterator(vec.begin()), vec.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with static_thread_pool, list, throwing iterator

  //  std::list<int> lst(10);

  //  std::static_thread_pool pool(4);

  //  for_each_n(par.on(pool.executor()), make_throwing_iterator(lst.begin()), lst.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  std::cout << "OK" << std::endl;

  return 0;
}

