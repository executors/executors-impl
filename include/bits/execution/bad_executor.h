#ifndef STD_BITS_EXECUTION_BAD_EXECUTOR_H_INCLUDED
#define STD_BITS_EXECUTION_BAD_EXECUTOR_H_INCLUDED

#include <exception>

namespace std {
namespace execution {

class bad_executor : std::exception
{
public:
  bad_executor() noexcept {}
  ~bad_executor() noexcept {}

  virtual const char* what() const noexcept
  {
    return "bad executor";
  }
};

} // namespace execution
} // namespace std

#endif // STD_BITS_EXECUTION_BAD_EXECUTOR_H_INCLUDED
