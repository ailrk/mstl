#pragma once

#include "miterator.hpp"
#include "mtype_traits.hpp"
#include "utility.hpp"

namespace mstl {
// find the first occurence of the value;
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

// this is similar to traverse_, but return the funtion applied, weirdly.
template <typename InputIter, typename UnaryFunction>
constexpr UnaryFunction for_each(InputIter first, InputIter last,
                                 UnaryFunction fn) {
  for (; first != last; ++first) {
    fn(*first);
  }

  return fn; // implicit move here.
}

// like take 5 . traverse_ maybe?
template <typename InputIt, typename Size, typename UnaryFunction>
constexpr InputIt for_each_n(InputIt first, Size n, UnaryFunction fn) {
  for (Size i = 0; i < n; ++i) {
    fn(*first++);
  }
  return first;
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

namespace mstlm {

// note: remove returns the past the end iterator, meaning the true
// end of the new sequence after elements get removed.
// e.g:
// 0 0 0 1 1 0 0 1
//       a b     c
// find will put first to position a then enter the loop.
// a, b all need to be removed, we pass them until iter rich (b + 1)
// we find it doesn't need to remove, then move it to the position of a.
// It's a very destructive algorithm.
template <typename ForwardIt, typename T>
constexpr ForwardIt remove(ForwardIt first, ForwardIt last, const T &value) {
  first = mstl::find(first, last, value);
  if (first != last) { // value exists

    for (ForwardIt iter = first; iter != last; ++iter) {
      if (*iter != value) {
        *first++ = mstl::move(*iter);
      }
    }
  }
  return first;
}

template <typename ForwardIt, typename T, typename UnaryPredicate>
constexpr ForwardIt remove_if(ForwardIt first, ForwardIt last,
                              UnaryPredicate pred) {
  first = mstl::find_if(first, last, pred);

  for (ForwardIt iter = first; iter != last; ++iter) {
    if (!pred(*iter)) {
      *first++ = mstl::move(*iter);
    }
  }

  return first;
}

} // namespace mstlm

namespace mstl {

template <typename InputIt, typename OutputIt, typename UnaryOperation>
constexpr OutputIt transform(InputIt first, InputIt last, OutputIt d_first,
                             UnaryOperation op) {
  while (first != last) {
    *d_first++ = op(*first++);
  }
  return d_first;
}

// oh why you have a zip here again?
template <typename InputIt1, typename InputIt2, typename OutputIt,
          typename BinaryOperator>
constexpr OutputIt transform(InputIt1 first1, InputIt1 last, InputIt2 first2,
                             OutputIt d_first, BinaryOperator op) {
  while (first1 != last) {
    *d_first++ = op(*first1++, *first2++);
  }
  return d_first;
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

namespace mstl {
// range function but also based on side effect, instead of return a new
// iterator.
template <typename ForwardIt, typename Generator>
void generate(ForwardIt first, ForwardIt last, Generator g) {
  while (first != last) {
    *first++ = g();
  }
}

template <typename OutputIt, typename Size, typename Generator>
OutputIt generate_n(OutputIt first, Size n, Generator g) {
  for (Size i = 0; i < n; ++i) {
    *first++ = g();
  }
  return first;
}

} // namespace mstl

namespace mstl {

// operator=  implies *first needs to be copyable.
template <typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
  while (first != last) {
    *d_first++ = *first++;
  }
  return d_first;
}

template <typename InputIt, typename OutputIt, typename UnaryPredicate>
constexpr OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first,
                           UnaryPredicate pred) {
  while (first != last) {
    if (pred(*first)) {
      *d_first++ = *first++;
    }
  }
  return d_first;
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt copy_n(InputIt first, Size count, OutputIt d_first) {
  if (count > 0) {
    for (Size n = 0; n < count; ++n) {
      *d_first++ = *first++;
    }
  }
  return d_first;
}

// copy from the last to the first.
template <typename BiIter1, typename BiIter2>
constexpr BiIter2 copy_backward(BiIter1 first1, BiIter1 last1, BiIter2 last2) {
  while (first1 != last1) {
    *(--last2) = *(--last1);
  }
  return last2;
}

} // namespace mstl

namespace mstl {

template <typename ForwardIt, typename T>
constexpr void fill(ForwardIt first, ForwardIt last, const T &value) {
  for (; first != last; ++first) {
    *first = value;
  }
}

template <typename OutputIt, typename Size, typename T>
constexpr OutputIt fill_n(OutputIt first, Size count, const T &value) {
  for (Size i = 0; i < count; ++i) {
    *first++ = value;
  }
  return first;
}

} // namespace mstl

namespace mstl {
// we can just move. note a move to b will call b's destructor.
template <typename T> void swap(T &a, T &b) {
  T tmp = mstl::move(a);
  a = mstl::move(b);
  b = mstl::move(tmp);
}

// nice thing is you can swap element between an array and a unordered map.
template <typename ForwardIter1, typename ForwardIter2>
void iter_swap(ForwardIter1 a, ForwardIter2 b) {
  mstl::swap(*a, *b);
}

// exchange elements from range [first1, last1) with another range starting from
// first2.
template <typename ForwardIt1, typename ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
  while (first1 != last1) {
    mstl::iter_swap(first1++, first2++);
  }
  return first2;
}

} // namespace mstl
