#pragma once

#include "miterator.hpp"
#include "mtype_traits.hpp"
#include "utility.hpp"

namespace mstl {
template <typename InputIter, typename T>
constexpr InputIter find(InputIter first, InputIter last, const T &value) {
  for (; first != last; ++first) {
    if (*first == value) {
      return first;
    }
  }
  return last;
}

template <typename InputIter, typename T, typename UnaryPredicate>
constexpr InputIter find_if(InputIter first, InputIter last,
                            UnaryPredicate pred) {
  for (; first != last; ++first) {
    if (pred(*first)) {
      return first;
    }
  }
  return last;
}

template <typename InputIter, typename T, typename UnaryPredicate>
constexpr InputIter find_if_not(InputIter first, InputIter last,
                                UnaryPredicate pred) {
  for (; first != last; ++first) {
    if (!pred(*first)) {
      return first;
    }
  }
  return last;
}

} // namespace mstl

namespace mstl {

template <typename InputIter, typename UnaryFunction>
constexpr UnaryFunction for_each(InputIter first, InputIter last,
                                 UnaryFunction fn) {
  for (; first != last; ++first) {
    fn(*first);
  }

  return fn; // implicit move here.
}

} // namespace mstl

namespace mstl {

// note: I looked up lots of sources about why it returns a difference_type
// but most explanaiton are vague and not seems right.
// One explaination was difference_type allows you to change the type used
// so you can have iterator with elements more than 64 bits allows to represent.
// But still, if that's the case, why difference_type though.
template <typename InputIter, typename T>
constexpr typename mstl::iterator_traits<InputIter>::difference_type
count(InputIter first, InputIter last, const T &value) {
  typename mstl::iterator_traits<InputIter>::difference_type ret = 0;
  for (; first != last; ++first) {
    ++ret;
  }
  return ret;
}

template <typename InputIter, typename T, typename UnaryPredicate>
constexpr typename mstl::iterator_traits<InputIter>::difference_type
count_if(InputIter first, InputIter last, UnaryPredicate pred) {
  typename mstl::iterator_traits<InputIter>::difference_type ret = 0;
  for (; first != last; ++first) {
    if (pred(*first)) {
      ++ret;
    }
  }
  return ret;
}

} // namespace mstl

namespace mstl {

template <typename InputIt, typename UnaryPredicate>
constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate pred) {
  return mstl::find_if_not(first, last, pred) == last;
}

template <typename InputIt, typename UnaryPredicate>
constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate pred) {
  return mstl::find_if(first, last, pred) != last;
}

template <typename InputIt, typename UnaryPredicate>
constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate pred) {
  return mstl::find_if(first, last, pred) == last;
}

} // namespace mstl
