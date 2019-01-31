#pragma

#include <utility>
#include <type_traits>

namespace impl
{


template<class Function>
class noexcept_function
{
  public:
    template<class OtherFunction,
             class = std::enable_if_t<
               std::is_constructible_v<Function,OtherFunction&&>
             >>
    noexcept_function(OtherFunction&& function) noexcept
      : function_(std::forward<OtherFunction>(function))
    {}

    noexcept_function(const noexcept_function& other) noexcept
      : function_(other.function_)
    {}

    noexcept_function(noexcept_function&& other) noexcept
      : function_(std::move(other.function_))
    {}

    ~noexcept_function() noexcept
    {}

    template<class... Args>
    auto operator()(Args&&... args) const noexcept
    {
      return function_(std::forward<Args>(args)...);
    }

  private:
    mutable Function function_;
};

template<class Function>
noexcept_function<std::decay_t<Function>> make_noexcept_function(Function&& f)
{
  return {std::forward<Function>(f)};
}


} // end impl

