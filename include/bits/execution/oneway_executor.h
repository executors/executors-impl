#ifndef STD_BITS_EXECUTION_ONEWAY_EXECUTOR_H_INCLUDED
#define STD_BITS_EXECUTION_ONEWAY_EXECUTOR_H_INCLUDED

#include <atomic>
#include <cassert>
#include <bits/execution/bad_executor.h>
#include <bits/execution/is_interface_property.h>
#include <memory>
#include <utility>

namespace std {
namespace execution {
namespace oneway_executor_impl {

template<class... SupportableProperties>
struct property_list;

template<>
struct property_list<> {};

template<class Head, class... Tail>
struct property_list<Head, Tail...> {};

template<class Property, class... SupportableProperties>
struct contains_exact_property;

template<class Property, class Head, class... Tail>
struct contains_exact_property<Property, Head, Tail...>
  : std::conditional<std::is_same<Property, Head>::value, std::true_type, contains_exact_property<Property, Tail...>>::type {};

template<class Property>
struct contains_exact_property<Property> : std::false_type {};

template<class Property, class... SupportableProperties>
static constexpr bool contains_exact_property_v = contains_exact_property<Property, SupportableProperties...>::value;

template<class Property, class... SupportableProperties>
struct contains_convertible_property;

template<class Property, class Head, class... Tail>
struct contains_convertible_property<Property, Head, Tail...>
  : std::conditional<std::is_convertible<Property, Head>::value, std::true_type, contains_convertible_property<Property, Tail...>>::type {};

template<class Property>
struct contains_convertible_property<Property> : std::false_type {};

template<class Property, class... SupportableProperties>
static constexpr bool contains_convertible_property_v = contains_convertible_property<Property, SupportableProperties...>::value;

template<class Property, class... SupportableProperties>
struct find_convertible_property;

template<class Property, class Head, class... Tail>
struct find_convertible_property<Property, Head, Tail...>
  : std::conditional<std::is_convertible<Property, Head>::value, std::decay<Head>, find_convertible_property<Property, Tail...>>::type {};

template<class Property>
struct find_convertible_property<Property> {};

template<class Property, class... SupportableProperties>
using find_convertible_property_t = typename find_convertible_property<Property, SupportableProperties...>::type;

template<class PropertyList, class... SupportableProperties>
struct contains_exact_property_list;

template<class Head, class... Tail, class... SupportableProperties>
struct contains_exact_property_list<property_list<Head, Tail...>, SupportableProperties...>
  : std::integral_constant<bool, contains_exact_property_v<Head, SupportableProperties...>
      && contains_exact_property_list<property_list<Tail...>, SupportableProperties...>::value> {};

template<class... SupportableProperties>
struct contains_exact_property_list<property_list<>, SupportableProperties...> : std::true_type {};

template<class PropertyList, class... SupportableProperties>
static constexpr bool contains_exact_property_list_v = contains_exact_property_list<PropertyList, SupportableProperties...>::value;

template<class Executor, class... SupportableProperties>
struct is_valid_target;

template<class Executor>
struct is_valid_target<Executor> : std::true_type {};

template<class Executor, class Head, class... Tail>
struct is_valid_target<Executor, Head, Tail...>
  : std::integral_constant<bool,
      (!Head::is_requirable_concept || can_require_concept_v<Executor, Head>)
      && (!Head::is_requirable || can_require_v<Executor, Head>)
      && (!Head::is_preferable || can_prefer_v<Executor, Head>)
      && (Head::is_requirable_concept || Head::is_requirable || Head::is_preferable || can_query_v<Executor, Head>)
      && is_valid_target<Executor, Tail...>::value> {};

template<class Executor, class... SupportableProperties>
static constexpr bool is_valid_target_v = is_valid_target<Executor, SupportableProperties...>::value;

struct identity_property
{
  static constexpr bool is_requirable = true;
  static constexpr bool is_preferable = true;
  template<class Executor> static constexpr bool static_query_v = true;
  static constexpr bool value() { return true; }
};

template<class Property, class... SupportableProperties>
using conditional_property_t = typename conditional<
  contains_exact_property_v<Property, SupportableProperties...>,
    Property, identity_property>::type;

template<class R, class... Args>
struct single_use_func_base
{
  virtual ~single_use_func_base() {}
  virtual R call(Args...) = 0;
};

template<class Function, class R, class... Args>
struct single_use_func : single_use_func_base<R, Args...>
{
  Function function_;

  explicit single_use_func(Function f) : function_(std::move(f)) {}

  virtual R call(Args... args)
  {
    std::unique_ptr<single_use_func> fp(this);
    Function f(std::move(function_));
    fp.reset();
    return f(std::forward<Args>(args)...);
  }
};

using oneway_func_base = single_use_func_base<void>;
template<class Function> using oneway_func = single_use_func<Function, void>;

struct impl_base
{
  virtual ~impl_base() {}
  virtual impl_base* clone() const noexcept = 0;
  virtual void destroy() noexcept = 0;
  virtual void execute(std::unique_ptr<oneway_func_base> f) = 0;
  virtual const type_info& target_type() const = 0;
  virtual void* target() = 0;
  virtual const void* target() const = 0;
  virtual bool equals(const impl_base* e) const noexcept = 0;
  virtual impl_base* require(const type_info&, const void* p) const = 0;
  virtual void* require_concept(const type_info&, const void* p) const = 0;
  virtual impl_base* prefer(const type_info&, const void* p) const = 0;
  virtual void* query(const type_info&, const void* p) const = 0;
};

template<class Executor, class... SupportableProperties>
struct impl : impl_base
{
  Executor executor_;
  std::atomic<std::size_t> ref_count_{1};

  explicit impl(Executor ex) : executor_(std::move(ex)) {}

  virtual impl_base* clone() const noexcept
  {
    impl* e = const_cast<impl*>(this);
    ++e->ref_count_;
    return e;
  }

  virtual void destroy() noexcept
  {
    if (--ref_count_ == 0)
      delete this;
  }

  virtual void execute(std::unique_ptr<oneway_func_base> f)
  {
    executor_.execute([f = std::move(f)]() mutable { f.release()->call(); });
  }

  virtual const type_info& target_type() const
  {
    return typeid(executor_);
  }

  virtual void* target()
  {
    return &executor_;
  }

  virtual const void* target() const
  {
    return &executor_;
  }

  virtual bool equals(const impl_base* e) const noexcept
  {
    if (this == e)
      return true;
    if (target_type() != e->target_type())
      return false;
    return executor_ == *static_cast<const Executor*>(e->target());
  }

  impl_base* require_helper(property_list<>, const type_info&, const void*) const
  {
    assert(0);
    return nullptr;
  }

  template<class Head, class... Tail>
  impl_base* require_helper(property_list<Head, Tail...>, const type_info& t, const void* p,
    typename std::enable_if<Head::is_requirable && !is_interface_property<Head>::value>::type* = 0) const
  {
    if (t == typeid(Head))
    {
      using executor_type = decltype(std::require(executor_, *static_cast<const Head*>(p)));
      return new impl<executor_type, SupportableProperties...>(std::require(executor_, *static_cast<const Head*>(p)));
    }
    return require_helper(property_list<Tail...>{}, t, p);
  }

  template<class Head, class... Tail>
  impl_base* require_helper(property_list<Head, Tail...>, const type_info& t, const void* p,
    typename std::enable_if<!Head::is_requirable || is_interface_property<Head>::value>::type* = 0) const
  {
    return require_helper(property_list<Tail...>{}, t, p);
  }

  virtual impl_base* require(const type_info& t, const void* p) const
  {
    return this->require_helper(property_list<SupportableProperties...>{}, t, p);
  }

  void* require_concept_helper(property_list<>, const type_info&, const void*) const
  {
    assert(0);
    return nullptr;
  }

  template<class Head, class... Tail>
  void* require_concept_helper(property_list<Head, Tail...>, const type_info& t, const void* p,
    typename std::enable_if<is_interface_property<Head>::value>::type* = 0) const
  {
    if (t == typeid(Head))
    {
      return new typename Head::template polymorphic_executor_type<>(
        typename Head::template polymorphic_executor_type<SupportableProperties...>(
          std::require_concept(executor_, *static_cast<const Head*>(p))));
    }
    return require_concept_helper(property_list<Tail...>{}, t, p);
  }

  template<class Head, class... Tail>
  void* require_concept_helper(property_list<Head, Tail...>, const type_info& t, const void* p,
    typename std::enable_if<!is_interface_property<Head>::value>::type* = 0) const
  {
    return require_concept_helper(property_list<Tail...>{}, t, p);
  }

  virtual void* require_concept(const type_info& t, const void* p) const
  {
    return this->require_concept_helper(property_list<SupportableProperties...>{}, t, p);
  }

  impl_base* prefer_helper(property_list<>, const type_info&, const void*) const
  {
    return clone();
  }

  template<class Head, class... Tail>
  impl_base* prefer_helper(property_list<Head, Tail...>, const type_info& t, const void* p,
    typename std::enable_if<Head::is_preferable && !is_interface_property<Head>::value>::type* = 0) const
  {
    if (t == typeid(Head))
    {
      using executor_type = decltype(std::prefer(executor_, *static_cast<const Head*>(p)));
      return new impl<executor_type, SupportableProperties...>(std::prefer(executor_, *static_cast<const Head*>(p)));
    }
    return prefer_helper(property_list<Tail...>{}, t, p);
  }

  template<class Head, class... Tail>
  impl_base* prefer_helper(property_list<Head, Tail...>, const type_info& t, const void* p,
    typename std::enable_if<!Head::is_preferable || is_interface_property<Head>::value>::type* = 0) const
  {
    return prefer_helper(property_list<Tail...>{}, t, p);
  }

  virtual impl_base* prefer(const type_info& t, const void* p) const
  {
    return this->prefer_helper(property_list<SupportableProperties...>{}, t, p);
  }

  void* query_helper(property_list<>, const type_info&, const void*) const
  {
    return nullptr;
  }

  template<class Head, class... Tail>
  void* query_helper(property_list<Head, Tail...>, const type_info& t, const void* p, typename std::enable_if<can_query_v<Executor, Head>>::type* = 0) const
  {
    if (t == typeid(Head))
      return new std::tuple<typename Head::polymorphic_query_result_type>(std::query(executor_, *static_cast<const Head*>(p)));
    return query_helper(property_list<Tail...>{}, t, p);
  }

  template<class Head, class... Tail>
  void* query_helper(property_list<Head, Tail...>, const type_info& t, const void* p, typename std::enable_if<!can_query_v<Executor, Head>>::type* = 0) const
  {
    return query_helper(property_list<Tail...>{}, t, p);
  }

  virtual void* query(const type_info& t, const void* p) const
  {
    return this->query_helper(property_list<SupportableProperties...>{}, t, p);
  }
};

} // namespace oneway_executor_impl

template<class... SupportableProperties>
class oneway_t::polymorphic_executor_type
{
public:
  // construct / copy / destroy:

  polymorphic_executor_type() noexcept
    : impl_(nullptr)
  {
  }

  polymorphic_executor_type(std::nullptr_t) noexcept
    : impl_(nullptr)
  {
  }

  polymorphic_executor_type(const polymorphic_executor_type& e) noexcept
    : impl_(e.impl_ ? e.impl_->clone() : nullptr)
  {
  }

  polymorphic_executor_type(polymorphic_executor_type&& e) noexcept
    : impl_(e.impl_)
  {
    e.impl_ = nullptr;
  }

  template<class Executor> polymorphic_executor_type(Executor e,
      typename std::enable_if<oneway_executor_impl::is_valid_target_v<
        typename std::enable_if<!std::is_same<Executor, polymorphic_executor_type>::value, Executor>::type,
          SupportableProperties...>>::type* = 0)
  {
    auto e2 = std::require_concept(std::move(e), oneway);
    impl_ = new oneway_executor_impl::impl<decltype(e2), SupportableProperties...>(std::move(e2));
  }

  template<class... OtherSupportableProperties>
  polymorphic_executor_type(polymorphic_executor_type<OtherSupportableProperties...> e,
      typename std::enable_if<oneway_executor_impl::contains_exact_property_list_v<
        oneway_executor_impl::property_list<SupportableProperties...>,
          OtherSupportableProperties...>>::type* = 0)
    : impl_(e.impl_ ? e.impl_->clone() : nullptr)
  {
  }

  template<class... OtherSupportableProperties>
  polymorphic_executor_type(polymorphic_executor_type<OtherSupportableProperties...> e,
      typename std::enable_if<!oneway_executor_impl::contains_exact_property_list_v<
        oneway_executor_impl::property_list<SupportableProperties...>,
          OtherSupportableProperties...>>::type* = 0) = delete;

  polymorphic_executor_type& operator=(const polymorphic_executor_type& e) noexcept
  {
    if (impl_) impl_->destroy();
    impl_ = e.impl_ ? e.impl_->clone() : nullptr;
    return *this;
  }

  polymorphic_executor_type& operator=(polymorphic_executor_type&& e) noexcept
  {
    if (this != &e)
    {
      if (impl_) impl_->destroy();
      impl_ = e.impl_;
      e.impl_ = nullptr;
    }
    return *this;
  }

  polymorphic_executor_type& operator=(nullptr_t) noexcept
  {
    if (impl_) impl_->destroy();
    impl_ = nullptr;
    return *this;
  }

  template<class Executor> polymorphic_executor_type& operator=(Executor e)
  {
    return operator=(polymorphic_executor_type(std::move(e)));
  }

  ~polymorphic_executor_type()
  {
    if (impl_) impl_->destroy();
  }

  // polymorphic executor modifiers:

  void swap(polymorphic_executor_type& other) noexcept
  {
    std::swap(impl_, other.impl_);
  }

  template<class Executor> void assign(Executor e)
  {
    operator=(polymorphic_executor_type(std::move(e)));
  }

  // polymorphic executor operations:

  template<class Property,
    class = typename std::enable_if<
      oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_requirable_concept
        && is_interface_property<Property>::value>::type>
  typename Property::template polymorphic_executor_type<SupportableProperties...> require_concept(const Property& p) const
  {
    oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...> p1(p);
    if (!impl_) throw bad_executor();
    return std::unique_ptr<typename Property::template polymorphic_executor_type<>>(
        static_cast<typename Property::template polymorphic_executor_type<>*>(
          impl_->require_concept(typeid(p1), &p1)))->template downcast<SupportableProperties...>();
  }

  template<class Property,
    class = typename std::enable_if<
      oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_requirable
        && !is_interface_property<Property>::value>::type>
  polymorphic_executor_type require(const Property& p) const
  {
    oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...> p1(p);
    return impl_ ? impl_->require(typeid(p1), &p1) : throw bad_executor();
  }

  template<class Property,
    class = typename std::enable_if<
      oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_preferable>::type>
  friend polymorphic_executor_type prefer(const polymorphic_executor_type& e, const Property& p)
  {
    oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...> p1(p);
    return e.get_impl() ? e.get_impl()->prefer(typeid(p1), &p1) : throw bad_executor();
  }

  template<class Property>
  auto query(const Property& p) const
    -> typename std::enable_if<
      oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_requirable_concept
        || oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_requirable
        || oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_preferable,
      typename oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::polymorphic_query_result_type>::type
  {
    oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...> p1(p);
    using result_type = typename decltype(p1)::polymorphic_query_result_type;
    using tuple_type = std::tuple<result_type>;
    if (!impl_) throw bad_executor();
    std::unique_ptr<tuple_type> result(static_cast<tuple_type*>(impl_->query(typeid(p1), &p1)));
    return result ? std::get<0>(*result) : result_type();
  }

  template<class Property>
  auto query(const Property& p) const
    -> typename std::enable_if<
      !oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_requirable_concept
        && !oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_requirable
        && !oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::is_preferable,
      typename oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...>::polymorphic_query_result_type>::type
  {
    oneway_executor_impl::find_convertible_property_t<Property, SupportableProperties...> p1(p);
    using result_type = typename decltype(p1)::polymorphic_query_result_type;
    using tuple_type = std::tuple<result_type>;
    if (!impl_) throw bad_executor();
    std::unique_ptr<tuple_type> result(static_cast<tuple_type*>(impl_->query(typeid(p1), &p1)));
    return std::get<0>(*result);
  }

  template<class Function>
  void execute(Function f) const
  {
    std::unique_ptr<oneway_executor_impl::oneway_func_base> fp(new oneway_executor_impl::oneway_func<Function>(std::move(f)));
    impl_ ? impl_->execute(std::move(fp)) : throw bad_executor();
  }

  // polymorphic executor capacity:

  explicit operator bool() const noexcept
  {
    return !!impl_;
  }

  // polymorphic executor target access:

  const type_info& target_type() const noexcept
  {
    return impl_ ? impl_->target_type() : typeid(void);
  }

  template<class Executor> Executor* target() noexcept
  {
    return impl_ ? static_cast<Executor*>(impl_->target()) : nullptr;
  }

  template<class Executor> const Executor* target() const noexcept
  {
    return impl_ ? static_cast<Executor*>(impl_->target()) : nullptr;
  }

  // polymorphic executor comparisons:

  friend bool operator==(const polymorphic_executor_type& a, const polymorphic_executor_type& b) noexcept
  {
    if (!a.get_impl() && !b.get_impl())
      return true;
    if (a.get_impl() && b.get_impl())
      return a.get_impl()->equals(b.get_impl());
    return false;
  }

  friend bool operator==(const polymorphic_executor_type& e, nullptr_t) noexcept
  {
    return !e;
  }

  friend bool operator==(nullptr_t, const polymorphic_executor_type& e) noexcept
  {
    return !e;
  }

  friend bool operator!=(const polymorphic_executor_type& a, const polymorphic_executor_type& b) noexcept
  {
    return !(a == b);
  }

  friend bool operator!=(const polymorphic_executor_type& e, nullptr_t) noexcept
  {
    return !!e;
  }

  friend bool operator!=(nullptr_t, const polymorphic_executor_type& e) noexcept
  {
    return !!e;
  }

  // polymorphic executor specialized algorithms:

  friend void swap(polymorphic_executor_type& a, polymorphic_executor_type& b) noexcept
  {
    a.swap(b);
  }

  template<class... OtherSupportableProperties>
  polymorphic_executor_type<OtherSupportableProperties...> downcast() const
  {
    return polymorphic_executor_type<OtherSupportableProperties...>(impl_->clone());
  }

private:
  template<class...> friend class oneway_t::polymorphic_executor_type;
  polymorphic_executor_type(oneway_executor_impl::impl_base* i) noexcept : impl_(i) {}
  oneway_executor_impl::impl_base* impl_;
  const oneway_executor_impl::impl_base* get_impl() const { return impl_; }
};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_ONEWAY_EXECUTOR_H_INCLUDED
