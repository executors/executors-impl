#pragma once

#include "query_or.hpp"
#include "noexcept_function.hpp"
#include "noexcept_iterator.hpp"
#include <experimental/execution>
#include <iterator>
#include <vector>


namespace impl
{


struct ignored {};
constexpr ignored ignore{};


template<class ExecutionPolicy, class RandomAccessIterator, class Size, class Function>
void for_each_n(std::random_access_iterator_tag, ExecutionPolicy&& policy, RandomAccessIterator first, Size n, Function f)
{
  // terminate if f throws for any reason
  auto noexcept_f = impl::make_noexcept_function(f);

  // terminate if an iterator operation throws for any reason
  auto noexcept_first = impl::make_noexcept_iterator(first);

  try
  {
    namespace execution = std::experimental::execution;

    // enforce requirements
    auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always);

    // create agents
    // note that this throws only upon failure to create EAs
    ex.bulk_execute(
      [=](size_t idx, impl::ignored&)
      {
        noexcept_f(noexcept_first[idx]);
      },
      n,
      []{ return impl::ignore; }
    );
  }
  catch(...)
  {
    // sequential fallback
    for(Size i = 0; i < n; ++i, ++noexcept_first)
    {
      noexcept_f(*noexcept_first);
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
  std::vector<ForwardIterator> result;
  result.reserve(num_subranges + 1);

  Size subrange_idx = 0;
  for(Size i = 0; i < n; i += subrange_size, ++subrange_idx)
  {
    // append the beginning of the subrange
    result.push_back(first);

    // are we at the last subrange?
    std::size_t distance = (subrange_idx == num_subranges - 1) ? (n - i) : subrange_size;

    std::advance(first, distance);
  }

  // finally, add the end of the range
  result.push_back(first);

  return result;
}


template<class ExecutionPolicy, class ForwardIterator, class Size, class Function>
void for_each_n(std::forward_iterator_tag, ExecutionPolicy&& policy, ForwardIterator first, Size n, Function f)
{
  // terminate if f throws for any reason
  auto noexcept_f = impl::make_noexcept_function(f);

  // terminate if an iterator operation throws for any reason
  auto noexcept_first = impl::make_noexcept_iterator(first);

  try
  {
    namespace execution = std::experimental::execution; 

    // enforce requirements
    auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always);

    // choose a number of subranges
    size_t num_subranges = std::min(n, query_or(ex, execution::occupancy, Size(1)));

    // create agents
    // note that this throws only upon failure to create EAs
    ex.bulk_execute(
      [=](size_t subrange_idx, const std::vector<noexcept_iterator<ForwardIterator>>& subranges_begin)
      {
        noexcept_iterator<ForwardIterator> end = subranges_begin[subrange_idx+1];

        for(noexcept_iterator<ForwardIterator> iter = subranges_begin[subrange_idx]; iter != end; ++iter)
        {
          noexcept_f(*iter);
        }
      },
      num_subranges,
      [=]
      {
        return impl::partition_into_subranges(noexcept_first, n, num_subranges);
      }
    );
  }
  catch(...)
  {
    // sequential fallback
    for(Size i = 0; i < n; ++i, ++noexcept_first)
    {
      noexcept_f(*noexcept_first);
    }
  }
}


} // end impl


template<class ExecutionPolicy, class Iterator, class Size, class Function>
void for_each_n(ExecutionPolicy&& policy, Iterator first, Size n, Function f)
{
  impl::for_each_n(typename std::iterator_traits<Iterator>::iterator_category(), std::forward<ExecutionPolicy>(policy), first, n, f);
}

