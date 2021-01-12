#pragma once

#include "mtype_traits.hpp"

namespace mstl {
template <typename T>
constexpr typename mstl::remove_reference<T>::type &&move(T &&t) noexcept {
  return static_cast<typename mstl::remove_reference<T>::type &&>(t);
}

// when forwarding a lvalue reference.
template <typename T>
constexpr T &&forward(typename mstl::remove_reference<T>::type &t) noexcept {
  return static_cast<T &&>(t);
}

// when forwarding a rvalue reference.
// note that forwarad<int&>(5) will be an compile time error.
// you can forward lvalue as rvalue, but not vice versa.
template <typename T>
constexpr T &&forward(typename mstl::remove_reference<T>::type &&t) noexcept {
  static_assert(!mstl::is_lvalue_reference<T>::value,
                "Cannot forward an rvalue as lvalue.");
  return static_cast<T &&>(t);
}

// To use decltype without creating a value.
template <typename T>
typename mstl::add_rvalue_reference<T>::type declval() noexcept;

} // namespace mstl
