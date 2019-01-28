#include <experimental/thread_pool>
#include <experimental/execution>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>


namespace execution = std::experimental::execution;
using std::experimental::static_thread_pool;


namespace impl
{


static_thread_pool system_thread_pool{std::max(1u,std::thread::hardware_concurrency())};


constexpr struct ignored {} ignore;


} // end impl


class parallel_policy
{
  public:
    static constexpr execution::bulk_guarantee_t::parallel_t execution_requirement{};
};


constexpr parallel_policy par{};


template<class RandomAccessIterator, class Size, class Function>
void for_each_n(const parallel_policy&, RandomAccessIterator first, Size n, Function f)
{
  auto ex = execution::require(impl::system_thread_pool.executor(), execution::bulk, execution::oneway, execution::blocking.always);

  try
  {
    // this only throws upon failure to create EAs
    ex.bulk_execute(
      [=](size_t idx, impl::ignored&)
      {
        try
        {
          f(first[idx]);
        }
        catch(...)
        {
          std::terminate();
        }
      },
      n,
      []{ return impl::ignore; }
    );
  }
  catch(...)
  {
    // sequential fallback
    for(Size i = 0; i < n; ++i, ++first)
    {
      f(*first);
    }
  }
}


int main()
{
  std::vector<int> vec(10);

  // test with par

  for_each_n(par, vec.begin(), vec.size(), [](int& x)
  {
    x = 42;
  });

  assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));

  std::cout << "OK" << std::endl;

  return 0;
}

