#pragma once

#include "malgorithm.hpp"
#include "mexceptoin.hpp"
#include <cstddef>

namespace mstl {

// you really use constexpr anywhere, no joke.
template <typename T, size_t N> struct array {

  T elements_[N];

  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = ptrdiff_t;
  using size_type = size_t;

  using iterator = value_type *;
  using const_iterator = const value_type *;

  using reverse_iterator = mstl::reverse_iterator<iterator>;
  using const_reverse_iterator = mstl::reverse_iterator<const_iterator>;

  constexpr value_type *data() noexcept { return elements_; }
  constexpr const value_type *data() const noexcept { return elements_; }

  constexpr void fill(const T &u) { mstl::fill_n(data(), data() + N, u); }
  constexpr void swap(array &a) {
    mstl::swap_ranges(data(), data() + N, a.data());
  }

  constexpr inline iterator begin() noexcept { return iterator(data()); }
  constexpr inline const_iterator begin() const noexcept {
    return const_iterator(data());
  }

  constexpr inline iterator end() noexcept { return iterator(data() + N); }
  constexpr inline const_iterator end() const noexcept {
    return const_iterator(data() + N);
  }

  constexpr inline reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  constexpr inline const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  constexpr inline reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  constexpr inline const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  // this just a shortcut ... it behaves exactly the same as begin();
  constexpr inline const_iterator cbegin() const noexcept { return begin(); }
  constexpr inline const_iterator cend() const noexcept { return end(); }

  constexpr inline const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }
  constexpr inline const_reverse_iterator crend() const noexcept {
    return rend();
  }

  constexpr size_type size() const noexcept { return N; }
  constexpr size_type max_size() const noexcept { return N; }

  constexpr bool empty() const noexcept { return N == 0; }

  constexpr reference operator[](size_type n) noexcept {
    static_assert(n < N, "mstl::array<T, N> index out of bound");
    return elements_[n];
  }

  constexpr reference at(size_type n) {
    if (n > N) {
      throw mstl::exception();
    }
    return elements_[n];
  }

};

} // namespace mstl
