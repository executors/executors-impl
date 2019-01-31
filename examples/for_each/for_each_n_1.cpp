#include "for_each_n.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>
#include <iostream>

int main()
{
  {
    // test with par, vector

    std::vector<int> vec(10);

    for_each_n(par, vec.begin(), vec.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));
  }

  {
    // test with par, list

    std::list<int> lst(10);

    for_each_n(par, lst.begin(), lst.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(lst.begin(), lst.end(), 42) == static_cast<int>(lst.size()));
  }

  // XXX the following tests correctly call std::terminate upon 
  //     exception in an element access function
  //     they are disabled to allow the test program to complete normally

  //{
  //  // test with par, vector, throwing function

  //  std::vector<int> vec(10);

  //  for_each_n(par, vec.begin(), vec.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, list, throwing function

  //  std::list<int> lst(10);

  //  for_each_n(par, lst.begin(), lst.size(), [](int& x)
  //  {
  //    x = 42;

  //    throw 13;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, vector, throwing iterator

  //  std::vector<int> vec(10);

  //  for_each_n(par, make_throwing_iterator(vec.begin()), vec.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, list, throwing iterator

  //  std::list<int> lst(10);

  //  for_each_n(par, make_throwing_iterator(lst.begin()), lst.size(), [](int& x)
  //  {
  //    x = 42;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  std::cout << "OK" << std::endl;

  return 0;
}

