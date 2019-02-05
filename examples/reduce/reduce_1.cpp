#include "reduce.hpp"
#include "par.hpp"
#include "throwing_iterator.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <functional>
#include <numeric>

int main()
{
  {
    // test with par, vector

    std::vector<int> vec(10);
    std::iota(vec.begin(), vec.end(), 1);

    int result = reduce(par, vec.begin(), vec.end(), 13, std::plus<>());

    assert(std::accumulate(vec.begin(), vec.end(), 13) == result);
  }

  {
    // test with par, list

    std::list<int> lst(10);
    std::iota(lst.begin(), lst.end(), 1);

    int result = reduce(par, lst.begin(), lst.end(), 13, std::plus<>());

    assert(std::accumulate(lst.begin(), lst.end(), 13) == result);
  }

  // XXX the following tests correctly call std::terminate upon 
  //     exception in an element access function
  //     they are disabled to allow the test program to complete normally

  //{
  //  // test with par, vector, throwing function

  //  std::vector<int> vec(10);
  //  std::iota(vec.begin(), vec.end(), 1);

  //  reduce(par, vec.begin(), vec.end(), 13, [](int x, int y)
  //  {
  //    throw 13;

  //    return x + y;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, list, throwing function

  //  std::list<int> lst(10);
  //  std::iota(lst.begin(), lst.end(), 1);

  //  reduce(par, lst.begin(), lst.end(), 13, [](int x, int y)
  //  {
  //    throw 13;

  //    return x + y;
  //  });

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, vector, throwing iterator

  //  std::vector<int> vec(10);
  //  std::iota(vec.begin(), vec.end(), 1);

  //  reduce(par, make_throwing_iterator(vec.begin()), make_throwing_iterator(vec.end()), 13, std::plus<>());

  //  // this line should never be reached
  //  assert(false);
  //}

  //{
  //  // test with par, list, throwing iterator

  //  std::list<int> lst(10);
  //  std::iota(lst.begin(), lst.end(), 1);

  //  reduce(par, make_throwing_iterator(lst.begin()), make_throwing_iterator(lst.end()), 13, std::plus<>());

  //  // this line should never be reached
  //  assert(false);
  //}

  std::cout << "OK" << std::endl;

  return 0;
}


