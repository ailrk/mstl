#pragma once
#include "mtype_traits.hpp"
#include "utility.hpp"
#include <cstddef>

#include <vector>
namespace mstl {

// these are essentially a product type.
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag {};

// iterator trait provides a uniform interface for iterators
//
// note: difference_type is a signed integer to identify the distance
// between iterators. This is generic type, and you can customize it
// for specific cases. For example you might have a huge sequence that
// 64bits can't represent the entire range (entries of mutiple databases etc.)
// so you can't use size_t in that case, but you can specify your own
// type.
template <typename Iter> struct iterator_traits {
  using difference_type = typename Iter::difference_type;
  using value_type = typename Iter::value_type;
  using pointer = typename Iter::pointer;
  using reference = typename Iter::reference;
  using iterator_category = typename Iter::iterator_category;
};

template <typename T> struct iterator_traits<T *> {
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = T *;
  using reference = T &;
  using iterator_category = mstl::random_access_iterator_tag;
};

template <typename Category, typename T, typename Distance = ptrdiff_t,
          typename Pointer = T *, typename Reference = T &>
struct iterator {
  using value_type = T;
  using difference_type = Distance;
  using pointer = Pointer;
  using reference = Reference;
  using iterator_category = Category;
};

} // namespace mstl
