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

// Note: before C++17 there was a type called std::iterator, which is used as
// the based class for other types of iterator to inherit. e.g
// template <typename T>
// class back_insert_iterator : public iterator<C, void, void, void, void> {...}
// this style was considered not clear, and now instead all implementations just
// list all elements directly.

} // namespace mstl

namespace mstl {

// iterator tag really works like an enum here.
template <typename Iter>
constexpr typename mstl::iterator_traits<Iter>::difference_type
distance__(Iter first, Iter last, mstl::input_iterator_tag) {
  typename mstl::iterator_traits<Iter>::difference_type ret = 0;
  for (; first != last; ++first, ++ret)
    ;

  return ret;
}

template <typename Iter>
constexpr typename mstl::iterator_traits<Iter>::difference_type
distance__(Iter first, Iter last, mstl::random_access_iterator_tag) {
  return last - first;
}

// return the distance between first and list.
// note ub: if the last is not reachable from the first.
template <typename Iter>
typename mstl::iterator_traits<Iter>::difference_type distance(Iter first,
                                                               Iter last) {
  return do_distance(first, last,
                     mstl::iterator_traits<Iter>::iterator_category());
}

} // namespace mstl

namespace mstl {

// the only purpose of back_insertor is to appaned value back
// to the container.
// Normally C's push_back will be called.
template <typename C> class back_insert_iterator {
protected:
  C *container;

public:
  using container_type = C;
  using difference_type = void; // these won't be used anyway
  using value_type = void;
  using pointer = void;
  using reference = void;
  using iterator_category = mstl::output_iterator_tag;

  explicit back_insert_iterator(C &x){};

  // you only wnat to push back bascially.
  back_insert_iterator &operator=(const typename C::value_type &value) {
    container->push_back(value);
    return *this;
  }
  back_insert_iterator &operator=(const typename C::value_type &&value) {
    container->push_back(mstl::move(value));
    return *this;
  }

  // increment just means return itself.
  back_insert_iterator &operator*() { return *this; };
  back_insert_iterator &operator++() { return *this; };
  back_insert_iterator operator++(int) { return *this; };
};

template <typename C> mstl::back_insert_iterator<C> back_inserter(C &x) {
  return back_insert_iterator<C>(x);
}

template <typename C> class front_insert_iterator {
protected:
  C *container;

public:
  using container_type = C;
  using difference_type = void; // these won't be used anyway
  using value_type = void;
  using pointer = void;
  using reference = void;
  using iterator_category = mstl::output_iterator_tag;

  explicit front_insert_iterator(C &x){};
  front_insert_iterator &operator=(const typename C::value_type &value) {
    container->push_front(value);
    return *this;
  }
  front_insert_iterator &operator=(const typename C::value_type &&value) {
    container->push_front(mstl::move(value));
    return *this;
  }
  front_insert_iterator &operator*() { return *this; };
  front_insert_iterator &operator++() { return *this; };
  front_insert_iterator operator++(int) { return *this; };
};

template <typename C> mstl::front_insert_iterator<C> back_inserter(C &x) {
  return front_insert_iterator<C>(x);
}

} // namespace mstl
