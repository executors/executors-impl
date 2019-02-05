#pragma once

#include <experimental/execution>
#include <type_traits>

template<class T, class Property, class Default,
         class = std::enable_if_t<
           std::experimental::execution::can_query_v<T,Property>
         >>
auto query_or(const T& query_me, const Property& prop, Default&&)
{
  return std::experimental::execution::query(query_me, prop);
}


template<class T, class Property, class Default,
         class = std::enable_if_t<
           !std::experimental::execution::can_query_v<T,Property>
         >>
Default&& query_or(const T&, const Property&, Default&& result)
{
  return std::forward<Default>(result);
}
