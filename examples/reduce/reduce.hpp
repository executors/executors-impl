#pragma once

#include <iostream>
#include "par.hpp"
#include "query_or.hpp"
#include "noexcept_function.hpp"
#include "noexcept_iterator.hpp"
#include <experimental/execution>
#include <utility>
#include <iterator>
#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>


namespace impl
{


template<class T>
struct reduce_state
{
  reduce_state(std::size_t num_partial_sums)
    : num_outstanding(num_partial_sums),
      sums(num_partial_sums)
  {}

  // XXX implement a move ctor to allow std::make_shared<reduce_state<T>>(shared_factory()) to work
  reduce_state(reduce_state&& other)
    : num_outstanding(other.sums.size()),
      sums(std::move(other.sums))
  {}

  std::atomic<int> num_outstanding;

  // XXX we should just make this a vector of optionals to avoid requiring DefaultConstructible T
  std::vector<T> sums;
};


template<class ExecutionPolicy, class RandomAccessIterator, class T, class BinaryOperation>
T reduce(std::random_access_iterator_tag, ExecutionPolicy&& policy, RandomAccessIterator first, RandomAccessIterator last, T init, BinaryOperation binary_op)
{
  std::optional<T> result;

  try
  {
    namespace execution = std::experimental::execution;

    // enforce requirements
    auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always);

    // measure the size of the range
    size_t n = last - first;

    // choose a number of subranges
    size_t num_subranges = std::min(n, query_or(ex, execution::occupancy, size_t(1)));

    // how large should each subrange be?
    size_t subrange_size = (n + num_subranges - 1) / num_subranges;

    // deduce the type of each partial sum
    using partial_sum_type = std::invoke_result_t<BinaryOperation, typename std::iterator_traits<RandomAccessIterator>::reference, typename std::iterator_traits<RandomAccessIterator>::reference>;

    // create agents
    ex.bulk_execute(
      [=,&result,&init](size_t subrange_idx, reduce_state<partial_sum_type>& shared)
      {
        // find the bounds of the subrange
        RandomAccessIterator subrange_begin = first + subrange_idx * subrange_size;
        RandomAccessIterator subrange_end = std::min(last, subrange_begin + subrange_size);

        // reduce the subrange to produce a partial sum
        shared.sums[subrange_idx] = std::accumulate(subrange_begin + 1, subrange_end, *subrange_begin, binary_op);

        // the final agent reduces partial sums
        if(--shared.num_outstanding == 0)
        {
          result = std::accumulate(shared.sums.begin(), shared.sums.end(), init, binary_op);
        }
      },
      num_subranges,
      [=]
      {
        return reduce_state<partial_sum_type>(num_subranges);
      }
    );
  }
  catch(...)
  {
    // sequential fallback
    result = std::accumulate(first, last, init, binary_op);
  }

  return result.value();
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


template<class ForwardIterator, class T>
struct partitions_and_reduce_state
{
  partitions_and_reduce_state(std::vector<ForwardIterator>&& partitions)
    : partitions_begin(std::move(partitions)),
      state(partitions_begin.size() - 1)
  {}

  partitions_and_reduce_state(partitions_and_reduce_state&& other)
    : partitions_begin(std::move(other.partitions_begin)),
      state(std::move(other.state))
  {}

  std::vector<ForwardIterator> partitions_begin;
  reduce_state<T> state;
};


template<class ExecutionPolicy, class ForwardIterator, class T, class BinaryOperation>
T reduce(std::forward_iterator_tag, ExecutionPolicy&& policy, ForwardIterator first, ForwardIterator last, T init, BinaryOperation binary_op)
{
  std::optional<T> result;

  try
  {
    namespace execution = std::experimental::execution;

    // enforce requirements
    auto ex = execution::require(policy.executor(), policy.execution_requirement, execution::bulk, execution::oneway, execution::blocking.always);

    // measure the size of the range
    size_t n = std::distance(first, last);

    // choose a number of subranges
    size_t num_subranges = std::min(n, query_or(ex, execution::occupancy, size_t(1)));

    // deduce the type of each partial sum
    using partial_sum_type = std::invoke_result_t<
      BinaryOperation,
      typename std::iterator_traits<ForwardIterator>::reference,
      typename std::iterator_traits<ForwardIterator>::reference
    >;

    // create agents
    ex.bulk_execute(
      [=,&result,&init](size_t subrange_idx, partitions_and_reduce_state<ForwardIterator,partial_sum_type>& shared)
      {
        // find the bounds of the subrange
        ForwardIterator subrange_begin = shared.partitions_begin[subrange_idx];
        ForwardIterator subrange_init = subrange_begin++;
        ForwardIterator subrange_end = shared.partitions_begin[subrange_idx + 1];

        // reduce the subrange to produce a partial sum
        shared.state.sums[subrange_idx] = std::accumulate(subrange_begin, subrange_end, *subrange_init, binary_op);

        // the final agent reduces partial sums
        if(--shared.state.num_outstanding == 0)
        {
          result = std::accumulate(shared.state.sums.begin(), shared.state.sums.end(), init, binary_op);
        }
      },
      num_subranges,
      [=]
      {
        // construct shared state for reduction
        return partitions_and_reduce_state<ForwardIterator,partial_sum_type>(partition_into_subranges(first, n, num_subranges));
      }
    );
  }
  catch(...)
  {
    // sequential fallback
    result = std::accumulate(first, last, init, binary_op);
  }

  return result.value();
}


} // end impl


template<class ExecutionPolicy, class ForwardIterator, class T, class BinaryOperation>
T reduce(ExecutionPolicy&& policy, ForwardIterator first, ForwardIterator last, T init, BinaryOperation binary_op)
{
  return impl::reduce(typename std::iterator_traits<ForwardIterator>::iterator_category(), std::forward<ExecutionPolicy>(policy), first, last, init, binary_op);
}


template<class ForwardIterator, class T, class BinaryOperation>
T reduce(const parallel_policy&, ForwardIterator first, ForwardIterator last, T init, BinaryOperation binary_op)
{
  return ::reduce(par.on(impl::system_thread_pool.executor()), first, last, init, binary_op);
}

