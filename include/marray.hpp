#pragma once

#include "malgorithm.hpp"
#include <cstddef>

namespace mstl {

template <typename T, size_t N> struct array {

  T elements_[N];

  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = ptrdiff_t;
  using size_type = size_t;

  using iterator = pointer;
  using const_iterator = const_pointer;

  constexpr value_type *data() noexcept { return elements_; }
  constexpr const value_type *data() const noexcept { return elements_; }

  constexpr void fill(const T &u) { mstl::fill_n(data(), data() + N, u); }
  constexpr void swap(array &);

  iterator begin() noexcept;
  const_iterator begin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;

  // TODO
};

} // namespace mstl
