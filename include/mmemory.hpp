#pragma once
#include "mtype_traits.hpp"
#include "utility.hpp"

namespace mstl {

// get the real address of object.
template <typename T> constexpr T *addressof(T &arg) noexcept {
  return reinterpret_cast<T *>(
      &const_cast<char &>(reinterpret_cast<const volatile char &>(arg)));
}
template <typename T>
constexpr T *addressof(const T &&arg) = delete; // no addr for const rvalue

} // namespace mstl


namespace mstl
{

} /* mstl */
