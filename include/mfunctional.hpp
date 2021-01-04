#pragma once

namespace mstl {

// helper functions for  reference_wrapper
template <typename T> constexpr T &FUN(T &t) noexcept { return t; }
template <typename T> void FUN(T &&) = delete;

// reference_wrapper wraps reference into an object, so you can store it around.
// reference_wrapper is copyable, thus you can make multiple aliases with it.
// note it is implicitly convertable to T&, so you can use it as a normal
// reference directly.
template <typename T> class reference_wrapper {
  T *ptr;

public:
  using type = T;
  // TODO need declval

};

} // namespace mstl
