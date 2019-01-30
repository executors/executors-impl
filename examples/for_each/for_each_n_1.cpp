#include <experimental/thread_pool>
#include <experimental/execution>
#include <vector>
#include <list>
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
void for_each_n_impl(std::random_access_iterator_tag, RandomAccessIterator first, Size n, Function f)
{
  auto ex = execution::require(impl::system_thread_pool.executor(), execution::bulk, execution::oneway, execution::blocking.always);

  try
  {
    // this only throws upon failure to create EAs
    // XXX will also throw if f or first's cctor throws
    //     we could wrap the lambda in a thing that terminates if anything throws
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


template<class ForwardIterator, class Size>
std::vector<ForwardIterator> partition_into_subranges(ForwardIterator first, Size n, std::size_t num_subranges)
{
  // how large should each subrange be?
  std::size_t subrange_size = (n + num_subranges - 1) / num_subranges;

  // store an iterator pointing to the beginning of each subrange
  // the final element of this vector points is first + n
  std::vector<ForwardIterator> result(num_subranges);

  Size subrange_idx = 0;
  for(Size i = 0; i < n; i += subrange_size, ++subrange_idx)
  {
    result[subrange_idx] = first;

    // are we at the last tile?
    std::size_t distance = (subrange_idx == num_subranges - 1) ? (n - i) : subrange_size;

    std::advance(first, distance);
  }

  // finally, add the end of the range
  result.push_back(first);

  return result;
}


template<class T, class Property, class Default,
         class = std::enable_if_t<
           execution::can_query_v<T,Property>
         >>
auto query_or(const T& query_me, const Property& prop, Default&&)
{
  return execution::query(query_me, prop);
}


template<class T, class Property, class Default,
         class = std::enable_if_t<
           !execution::can_query_v<T,Property>
         >>
Default&& query_or(const T&, const Property&, Default&& result)
{
  return std::forward<Default>(result);
}



template<class ForwardIterator, class Size, class Function>
void for_each_n_impl(std::forward_iterator_tag, ForwardIterator first, Size n, Function f)
{
  try
  {
    // enforce requirements
    auto ex = execution::require(impl::system_thread_pool.executor(), execution::bulk, execution::oneway, execution::blocking.always);

    // choose a number of subranges
    size_t num_subranges = std::min(n, query_or(ex, execution::occupancy, 1));

    // create agents
    ex.bulk_execute(
      [=](size_t subrange_idx, const std::vector<ForwardIterator>& subranges_begin)
      {
        ForwardIterator end = subranges_begin[subrange_idx+1];

        for(ForwardIterator iter = subranges_begin[subrange_idx]; iter != end; ++iter)
        {
          f(*iter);
        }
      },
      num_subranges,
      [=]
      {
        return partition_into_subranges(first, n, num_subranges);
      }
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


template<class Iterator, class Size, class Function>
void for_each_n(const parallel_policy&, Iterator first, Size n, Function f)
{
  for_each_n_impl(typename std::iterator_traits<Iterator>::iterator_category(), first, n, f);
}


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

  std::cout << "OK" << std::endl;

  return 0;
}

