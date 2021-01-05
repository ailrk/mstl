#pragma once

#include "mtype_traits.hpp"

namespace mstl {

// exception base class.
// exception is one of few places the stl actually use inheritence.
// it makes sense because exceptions forms a heirachy naturally.
class exception {
public:
  exception() noexcept {}
  exception(const exception &) noexcept = default;

  virtual ~exception() noexcept;
  virtual const char *what() const noexcept;
};

class bad_exception : public exception {
public:
  bad_exception() noexcept {}
  virtual ~bad_exception() noexcept;
  virtual const char *what() const noexcept;
};

} // namespace mstl
