#ifndef STD_BITS_PROPERTIES_PREFER_ONLY_H_INCLUDED
#define STD_BITS_PROPERTIES_PREFER_ONLY_H_INCLUDED

namespace std {
namespace prefer_only_impl {

template<class>
struct type_check
{
  typedef void type;
};

template<class InnerProperty, class = void>
struct prefer_only_base {};

template<class InnerProperty>
struct prefer_only_base<InnerProperty,
  typename type_check<typename InnerProperty::polymorphic_query_result_type>::type>
{
  using polymorphic_query_result_type =
    typename InnerProperty::polymorphic_query_result_type;
};

template<class, class InnerProperty>
inline auto property_value(const InnerProperty& property)
  noexcept(noexcept(std::declval<const InnerProperty>().value()))
    -> decltype(std::declval<const InnerProperty>().value())
{
  return property.value();
}

} // namespace prefer_only_impl

template<class InnerProperty>
struct prefer_only : prefer_only_impl::prefer_only_base<InnerProperty>
{
  InnerProperty property;

  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = InnerProperty::is_preferable;

  template<class Entity, class Type = decltype(InnerProperty::template static_query_v<Entity>)>
    static constexpr Type static_query_v = InnerProperty::template static_query_v<Entity>;

  constexpr prefer_only(const InnerProperty& p) : property(p) {}

  template<class Dummy = int>
  constexpr auto value() const
    noexcept(noexcept(prefer_only_impl::property_value<Dummy, InnerProperty>()))
      -> decltype(prefer_only_impl::property_value<Dummy, InnerProperty>())
  {
    return prefer_only_impl::property_value(property);
  }

  template<class Entity, class Property, class = typename std::enable_if<std::is_same<Property, prefer_only>::value>::type>
  friend auto prefer(Entity ex, const Property& p)
    noexcept(noexcept(std::prefer(std::move(ex), std::declval<const InnerProperty>())))
      -> decltype(std::prefer(std::move(ex), std::declval<const InnerProperty>()))
  {
    return std::prefer(std::move(ex), p.property);
  }

  template<class Entity, class Property, class = typename std::enable_if<std::is_same<Property, prefer_only>::value>::type>
  friend constexpr auto query(const Entity& ex, const Property& p)
    noexcept(noexcept(std::query(ex, std::declval<const InnerProperty>())))
    -> decltype(std::query(ex, std::declval<const InnerProperty>()))
  {
    return std::query(ex, p.property);
  }
};

} // namespace std

#endif // STD_BITS_PROPERTIES_PREFER_ONLY_H_INCLUDED
