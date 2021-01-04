#pragma once
#include "utility.hpp"
namespace mstl {

// iota is a side effect based function.
// instead of return a range, it fill the iterator with
// value incrementally.
template <typename ForwardIt, typename T>
constexpr void iota(ForwardIt first, ForwardIt last, T value) {
  while (first != last) {
    *first++ = value;
    ++value;
  }
}

// accumulate is fold left. but if you want to use a right fold,
// you can just pass a reverse iterator.
// Because in c++ everything is egerly evaluated, there is no
// difference between foldl and foldr other the order.
template <typename InputIter, typename T>
constexpr T accumulate(InputIter first, InputIter last, T init) {
  for (; first != last; ++first) {
    init = mstl::move(init) + *first;
  }
  return init;
}

template <typename InputIter, typename T, typename BinaryOperator>
constexpr T accumulate(InputIter first, InputIter last, T init,
                       BinaryOperator op) {
  for (; first != last; ++first) {
    init = op(mstl::move(init), *first);
  }
  return init;
}

// it's really just zip. The default case just happened act like inner product
// of vectors.
template <typename Iter1, typename Iter2, typename T>
constexpr T inner_product(Iter1 first1, Iter1 last1, Iter2 first2, T init) {
  while (first1 != last1) {
    init = mstl::move(init) + *first1 * *first2;
    ++first1;
    ++first2;
  }

  return init;
}

template <typename Iter1, typename Iter2, typename T, typename BinaryOperator1,
          typename BinaryOperator2>
constexpr T inner_product(Iter1 first1, Iter1 last1, Iter2 first2, T init,
                          BinaryOperator1 op1, BinaryOperator2 op2) {
  while (first1 != last1) {
    init = op1(mstl::move(init), op2(*first1, *first2));
    ++first1;
    ++first2;
  }

  return init;
}

} // namespace mstl
