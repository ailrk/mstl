#pragma once

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

  constexpr void fill(const T &);
  constexpr void swap(array &);

  iterator begin() noexcept;
  const_iterator begin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;


};

} // namespace mstl
