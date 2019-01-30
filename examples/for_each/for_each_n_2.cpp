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


template<class, class> class basic_execution_policy;


template<class ER, class E>
basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex);


template<class ExecutionRequirement, class Executor>
class basic_execution_policy
{
  public:
    static_assert(execution::can_require_v<Executor, execution::bulk_t, ExecutionRequirement>);

    static constexpr ExecutionRequirement execution_requirement{};

    Executor executor() const
    {
      return executor_;
    }

  private:
    template<class ER, class E>
    friend basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex);

    basic_execution_policy(const Executor& executor)
      : executor_(executor)
    {}

    Executor executor_;
};


template<class ER, class E>
basic_execution_policy<ER,E> make_basic_execution_policy(const E& ex)
{
  return basic_execution_policy<ER,E>{ex};
}


constexpr struct ignored {} ignore;


template<class Executor, class ExecutionRequirement>
constexpr bool satisfies_cpp20_on_requirements_v =
  execution::can_require_v<
    Executor
    , ExecutionRequirement
    , execution::bulk_t
    , execution::oneway_t
    , execution::blocking_t::always_t
    , execution::mapping_t::thread_t
>;


} // end impl


class parallel_policy
{
  public:
    static constexpr execution::bulk_guarantee_t::parallel_t execution_requirement{};

    template<class Executor,
             class = std::enable_if_t<
               impl::satisfies_cpp20_on_requirements_v<Executor, decltype(execution_requirement)>
             >>
    impl::basic_execution_policy<decltype(execution_requirement), Executor> on(const Executor& ex) const
    {
      return impl::make_basic_execution_policy<decltype(execution_requirement)>(ex);
    }
};


constexpr parallel_policy par{};


template<class ExecutionPolicy, class RandomAccessIterator, class Size, class Function>
void for_each_n_impl(std::random_access_iterator_tag, ExecutionPolicy&& policy, RandomAccessIterator first, Size n, Function f)
{
  try
  {
    // enforce requirements
    auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always, execution::mapping.thread);

    // create agents
    ex.bulk_execute(
      [=](size_t idx, impl::ignored&)
      {
        f(first[idx]);
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


template<class ForwardIterator, class Size>
std::vector<ForwardIterator> partition_into_subranges(ForwardIterator first, Size n, std::size_t num_subranges)
{
  ForwardIterator first_ = first;

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


template<class ExecutionPolicy, class ForwardIterator, class Size, class Function>
void for_each_n_impl(std::forward_iterator_tag, ExecutionPolicy&& policy, ForwardIterator first, Size n, Function f)
{
  try
  {
    // enforce requirements
    auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always, execution::mapping.thread);

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


template<class ExecutionPolicy, class Iterator, class Size, class Function>
void for_each_n(ExecutionPolicy&& policy, Iterator first, Size n, Function f)
{
  for_each_n_impl(typename std::iterator_traits<Iterator>::iterator_category(), std::forward<ExecutionPolicy>(policy), first, n, f);
}


int main()
{
  {
    // test with static_thread_pool, vector

    std::vector<int> vec(10);

    std::experimental::static_thread_pool pool(4);

    for_each_n(par.on(pool.executor()), vec.begin(), vec.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));
  }

  {
    // test with static_thread_pool, list

    std::list<int> lst(10);

    std::experimental::static_thread_pool pool(4);

    for_each_n(par.on(pool.executor()), lst.begin(), lst.size(), [](int& x)
    {
      x = 42;
    });

    assert(std::count(lst.begin(), lst.end(), 42) == static_cast<int>(lst.size()));
  }

  std::cout << "OK" << std::endl;

  return 0;
}

