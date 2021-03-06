#pragma once
#include "miterator.hpp"
#include "mtype_traits.hpp"
#include <cstddef>

namespace mstl {

// An initializer list is essentially a slice of some elements.
// core util: a pointer to the head of the list and the size of the list.
// Initialize with initializer list is really just pass the pointer and
// size of the list to the container, and let the container takes over the
// ownership.
// One thing I don't understand is that initializer list doesn't own the
// underlying array. The cppreference says the underlying array of initalizer
// list is always a temporary, and the detail is unspecified.  When is that
// array get allocated?
// My guess is it's compiler support mechanism, and initializer list here is
// just an handle over the temporary array. But I can't find evidence to verify
// that.
template <typename T> class initializer_list {

  const T *begin_;
  const size_t size_;
  constexpr initializer_list(const T *b, size_t s) : begin_(b), size_(s) {}

public:
  using value_type = T;
  using reference = const T &;
  using const_reference = const T &;
  using size_type = size_t;

  using iterator = const T *;
  using const_iterator = const T *;

  constexpr initializer_list() noexcept : begin_(nullptr), size_(0) {}

  constexpr size_t size() const noexcept { return size_; };
  constexpr T *begin() const noexcept { return begin_; }
  constexpr T *end() const noexcept { return begin_ + size_; }
};

template <typename T>
inline const T *begin(mstl::initializer_list<T> ls) noexcept {
  return ls.begin();
}

template <typename T>
inline const T *end(mstl::initializer_list<T> ls) noexcept {
  return ls.end();
}

} // namespace mstl
