#ifndef STD_BITS_EXECUTION_EXECUTOR_CONCEPT_H_INCLUDED
#define STD_BITS_EXECUTION_EXECUTOR_CONCEPT_H_INCLUDED

namespace std {
namespace execution {
namespace executor_concept_impl {

template<class Derived>
struct property_base
{
  static constexpr bool is_requirable_concept = false;
  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  template<class Executor, class Type = decltype(Executor::query(*static_cast<Derived*>(0)))>
    static constexpr Type static_query_v = Executor::query(Derived());
};

} // namespace executor_concept_impl

struct executor_concept_t : executor_concept_impl::property_base<executor_concept_t> {};

constexpr executor_concept_t executor_concept;

} // namespace execution

template<class Entity>
struct is_applicable_property<Entity, execution::executor_concept_t> : std::true_type {};

} // namespace std

#endif // STD_BITS_EXECUTION_EXECUTOR_CONCEPT_H_INCLUDED
