#pragma once
#include "mmemory.hpp"
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
  return distance__(first, last,
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

namespace mstl {

// reverse iterator is an iterator adaptor, it requires the underlying iterator
// to be a bidirectional iterator. Reverse iterator builds a new iterator on top
// of the underlying iterator. Assume the following iterator:
//   1      2     3     4     5    _
//   begin                        end
// Reverse iterator manage two new pointers as this:
//   _      1      2     3     4     5
//   rend                           rbegin
// You can access the underlying iterator with base();
// Notice the subtlty of + and -
// - gives difference_type yet + gives a new iterator.
// - can have the case that iter - iter, but for +, both operands must be iter.
// it makes sense if you view iters as pointers.
//
// &*r == &*(i-1)
template <typename Iter> class reverse_iterator {

protected:
  Iter current;

public:
  using difference_type = typename mstl::iterator_traits<Iter>::difference_type;
  using value_type = typename mstl::iterator_traits<Iter>::value_type;
  using pointer = typename mstl::iterator_traits<Iter>::pointer;
  using reference = typename mstl::iterator_traits<Iter>::reference;
  using iterator_category =
      typename mstl::iterator_traits<Iter>::iterator_category;

  // default initialize the iterator
  constexpr reverse_iterator() : current() {}

  constexpr explicit reverse_iterator(Iter x) : current(x) {}

  // copy constructors.
  template <typename U>
  constexpr reverse_iterator(const mstl::reverse_iterator<U> &u)
      : current(u.base()) {}

  template <typename U>
  constexpr reverse_iterator &operator=(const reverse_iterator<U> &u) {
    this->current = u.base();
    return *this;
  }

  constexpr Iter base() const { return current; }

  // when refering the last element, reverse iterator actually refers to
  // one before it.
  // 1 2 3 4 _
  //         ^
  constexpr reference operator*() const {
    Iter tmp = current;
    return *--tmp;
  }

  // address of the element returned by operator*
  constexpr pointer operator->() const { return mstl::addressof(operator*()); }

  constexpr reverse_iterator &operator++() {
    --current;
    return *this;
  }

  // this one return a copy
  constexpr reverse_iterator operator++(int) {
    reverse_iterator tmp(*this);
    --current;
    return tmp;
  }

  constexpr reverse_iterator &operator--() {
    ++current;
    return *this;
  }

  constexpr reverse_iterator operator--(int) {
    reverse_iterator tmp(*this);
    ++current;
    return tmp;
  }

  // return by value if it creates a new iterator. otherwise return a
  // reference of itself.
  // left +
  constexpr reverse_iterator operator+(difference_type n) const {
    return reverse_iterator(current - n);
  }

  constexpr reverse_iterator &operator+=(difference_type n) {
    current -= n;
    return *this;
  }

  constexpr reverse_iterator operator-(difference_type n) const {
    return reverse_iterator(current + n);
  }

  constexpr reverse_iterator &operator-=(difference_type n) {
    current += n;
    return *this;
  }

  // reuse functions defined above
  constexpr reference operator[](difference_type n) const {
    return *(*this + n);
  }
};

template <typename Iter1, typename Iter2>
constexpr bool operator==(const mstl::reverse_iterator<Iter1> &x,
                          const mstl::reverse_iterator<Iter2> &y) {
  return x.base() == y.base();
}

template <typename Iter1, typename Iter2>
constexpr bool operator<(const mstl::reverse_iterator<Iter1> &x,
                         const mstl::reverse_iterator<Iter2> &y) {

  return x.base() > y.base();
}

template <typename Iter1, typename Iter2>
constexpr bool operator<=(const mstl::reverse_iterator<Iter1> &x,
                          const mstl::reverse_iterator<Iter2> &y) {
  return x.base() >= y.base();
}

template <typename Iter1, typename Iter2>
constexpr bool operator!=(const mstl::reverse_iterator<Iter1> &x,
                          const mstl::reverse_iterator<Iter2> &y) {
  return !(x == y);
}

template <typename Iter1, typename Iter2>
constexpr bool operator>(const mstl::reverse_iterator<Iter1> &x,
                         const mstl::reverse_iterator<Iter2> &y) {
  return x.base() < y.base();
}

template <typename Iter1, typename Iter2>
constexpr bool operator>=(const mstl::reverse_iterator<Iter1> &x,
                          const mstl::reverse_iterator<Iter2> &y) {

  return x.base() >= y.base();
}

template <typename Iter1, typename Iter2>
constexpr auto operator-(const mstl::reverse_iterator<Iter1> &x,
                         const mstl::reverse_iterator<Iter2> &y)
    -> decltype(y.base() - x.base()) {
  return y.base() - x.base();
}

// right plus
// just advance the iterator
template <typename Iter>
constexpr mstl::reverse_iterator<Iter>
operator+(typename mstl::reverse_iterator<Iter>::difference_type n,
          const mstl::reverse_iterator<Iter> &x) {
  return reverse_iterator<Iter>(x.base() - n);
}

template <typename Iter>
constexpr reverse_iterator<Iter> make_reverse_iterator(Iter i) {

  return reverse_iterator<Iter>(i);
}

} // namespace mstl

namespace mstl {

// move iterator is another iterator adaptor. The idea is if a move
// iterator is used as an output iterator, the value will be moved
// instead of copied.

} // namespace mstl

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// TODO remove these dependencies.
#include <deque>
#include <functional>
#include <iostream>
#include <stack>
#include <vector>

namespace mstl {

// Monotonic queue for sliding window problems.
// find the biggest value in a subsequence
//
// O(n) for querying extrema in an interval. It's better then ST table and
// segment tree.
//
// The idea is to maintain a deque, and only include elements that are
// `possible` to be the biggest (smallest) value in the subsequence.

struct monotonic_increasing {};
struct monotonic_decreasing {};

template <typename Tag, typename = void>
struct is_monotonic_iterator_tag : mstl::false_type {};

template <typename Tag>
struct is_monotonic_iterator_tag<
    Tag, mstl::void_t<typename mstl::enable_if<std::conjunction_v<
             mstl::is_same<Tag, monotonic_increasing>::value,
             mstl::is_same<Tag, monotonic_decreasing>::value>>::type>>
    : mstl::true_type {};

template <typename Iter, typename Comp> class monotonic_queue_iterator {
private:
  using T = typename mstl::iterator_traits<Iter>::value_type;
  mstl::deque<T> queue;
  size_t win_size;
  Iter first;

  Comp comp;

public:
  using difference_type = void;
  using value_type = mstl::deque<T>;
  using pointer = mstl::deque<T> *;
  using reference = mstl::deque<T> &;
  using const_reference = const mstl::deque<T> &;
  using iterator_category = mstl::input_iterator_tag;

  // initialize the first window.
  explicit constexpr monotonic_queue_iterator(Iter first, size_t window_size,
                                              const Comp &comp)
      : win_size(window_size), first(first), comp(comp) {
    auto it = first;
    auto max_it = it;

    for (size_t i = 0; i < win_size; ++i) {
      mstl::cout << *it << mstl::endl;
      if (comp(*it, *max_it)) {
        max_it = it;
      }

      if (i < win_size - 1)
        ++it;
    }

    queue.push_back(*max_it);

    if (max_it != it) {
      queue.push_back(*it);
    }
  }

  explicit constexpr monotonic_queue_iterator(Iter first, size_t win_size,
                                              monotonic_decreasing g)
      : monotonic_queue_iterator(
            first, win_size,
            [](const auto &a, const auto &b) { return a > b; }) {}

  explicit constexpr monotonic_queue_iterator(Iter first, size_t win_size,
                                              monotonic_increasing g)
      : monotonic_queue_iterator(
            first, win_size,
            [](const auto &a, const auto &b) { return a < b; }) {}

  constexpr inline friend bool
  operator==(const monotonic_queue_iterator &self,
             const monotonic_queue_iterator &other) noexcept {
    return self.win_size == other.win_size && self.first == other.first;
  }

  constexpr inline friend bool
  operator!=(const monotonic_queue_iterator &self,
             const monotonic_queue_iterator &other) noexcept {
    return !(self == other);
  }

  constexpr inline monotonic_queue_iterator<Iter, Comp> operator++() {
    auto it = ++first;
    mstl::advance(it, win_size - 1);

    auto qit = queue.rbegin();
    while (!queue.empty() && qit != queue.rend() && *qit < *it) {
      qit++;
    }

    queue.erase(qit.base(), queue.end());
    queue.push_back(*it);

    return *this;
  }

  constexpr inline monotonic_queue_iterator<Iter, Comp> operator++(int) {
    monotonic_queue_iterator tmp(*this);
    ++this;
    return tmp;
  }

  constexpr inline const_reference operator*() const noexcept { return queue; }
  constexpr inline const_reference operator*() noexcept { return queue; }

  constexpr pointer operator->() noexcept {
    return mstl::addressof(operator*());
  }
};

template <typename Iter, typename Comp, typename Tag,
          typename mstl::enable_if_t<is_monotonic_iterator_tag<Tag>::value>>
constexpr decltype(auto) make_monotonic_queue_iterators(Iter begin, Iter end,
                                                        size_t window_size,
                                                        Tag tag) {
  return mstl::make_pair(
      monotonic_queue_iterator{begin, window_size, tag},
      monotonic_queue_iterator{end - window_size + 1, window_size, tag});
}

template <typename Iter, typename Comp>
constexpr decltype(auto) make_monotonic_queue_iterators(Iter begin, Iter end,
                                                        size_t window_size,
                                                        const Comp &comp) {
  return mstl::make_pair(
      monotonic_queue_iterator{begin, window_size, comp},
      monotonic_queue_iterator{end - window_size + 1, window_size, comp});
}

template <typename Iter, typename Comp>
constexpr decltype(auto) make_monotonic_queue_iterators(Iter begin, Iter end,
                                                        size_t window_size) {
  return mstl::make_pair(
      monotonic_queue_iterator{begin, window_size, monotonic_decreasing()},
      monotonic_queue_iterator{end - window_size + 1, window_size,
                               monotonic_decreasing()});
}

template <typename C, typename Comp>
constexpr decltype(auto) make_monotonic_queue_iterators(C container,
                                                        size_t window_size,
                                                        const Comp &comp) {

  return mstl::make_pair(
      monotonic_queue_iterator{mstl::begin(container), window_size, comp},
      monotonic_queue_iterator{mstl::end(container) - window_size + 1,
                               window_size, comp});
}

template <typename Iter, typename Tag>
monotonic_queue_iterator(Iter, size_t, Tag) -> monotonic_queue_iterator<
    Iter,
    mstl::function<bool(typename mstl::iterator_traits<Iter>::value_type,
                        typename mstl::iterator_traits<Iter>::value_type)>>;

template <typename Iter>
monotonic_queue_iterator(Iter, size_t) -> monotonic_queue_iterator<
    Iter,
    mstl::function<bool(typename mstl::iterator_traits<Iter>::value_type,
                        typename mstl::iterator_traits<Iter>::value_type)>>;

// monotonic stack
// to solve NGE (next greater element) problem in O(n)
// find the next greater element for all elements.
// stack top is always the closest to the new element. If the new element is
// bigger then the stack top, then it's the NGE of it.

template <typename Iter, typename Comp> class monotonic_stack_iterator {
private:
  using T = typename mstl::iterator_traits<Iter>::value_type;
  using stack = mstl::vector<T>;
  Comp comp;
  stack data;
  Iter first;

public:
  using difference_type = void;
  using pointer = stack *;
  using reference = stack &;
  using const_reference = const stack &;
  using value_type = T;
  using iterator_category = mstl::input_iterator_tag;

  constexpr monotonic_stack_iterator(Iter first, const Comp &comp)
      : comp(comp), data(), first(first) {}

  constexpr monotonic_stack_iterator(Iter first, monotonic_decreasing tag)
      : monotonic_stack_iterator(
            first, [](const auto &a, const auto &b) { return a > b; }) {}

  constexpr monotonic_stack_iterator(Iter first, monotonic_increasing tag)
      : monotonic_stack_iterator(
            first, [](const auto &a, const auto &b) { return a < b; }) {}

  constexpr friend bool operator==(const monotonic_stack_iterator &self,
                                   const monotonic_stack_iterator &other) {
    return self.first == other.first;
  }

  constexpr friend bool operator!=(const monotonic_stack_iterator &self,
                                   const monotonic_stack_iterator &other) {
    return !(self == other);
  }

  constexpr monotonic_stack_iterator<Iter, Comp> operator++() {
    auto it = first++;
    if (data.empty()) {
      data.push_back(*it);
      return *this;
    }

    while (!data.empty() && comp(*it, data.back())) {
      data.pop_back();
    }

    data.push_back(*it);
    return *this;
  }

  constexpr monotonic_stack_iterator<Iter, Comp> operator++(int) {
    monotonic_stack_iterator tmp(*this);
    ++this;
    return tmp;
  }

  constexpr const_reference operator*() { return data; }
  constexpr const_reference operator*() const { return data; }
};

template <typename Iter>
monotonic_stack_iterator(Iter) -> monotonic_stack_iterator<
    Iter,
    mstl::function<bool(typename mstl::iterator_traits<Iter>::value_type,
                        typename mstl::iterator_traits<Iter>::value_type)>>;

template <typename Iter, typename Tag>
monotonic_stack_iterator(Iter, Tag) -> monotonic_stack_iterator<
    Iter,
    mstl::function<bool(typename mstl::iterator_traits<Iter>::value_type,
                        typename mstl::iterator_traits<Iter>::value_type)>>;

template <typename Iter, typename Comp>
constexpr decltype(auto) make_monotonic_stack_iterators(Iter begin, Iter end,
                                                        const Comp &comp) {
  return mstl::pair(monotonic_stack_iterator{begin, comp},
                    monotonic_stack_iterator{end + 1, comp});
}

template <typename Iter, typename Comp, typename Tag,
          typename mstl::enable_if_t<is_monotonic_iterator_tag<Tag>::value>>
constexpr decltype(auto) make_monotonic_stack_iterators(Iter begin, Iter end,
                                                        Tag tag) {
  return mstl::pair(monotonic_stack_iterator{begin, tag},
                    monotonic_stack_iterator{end + 1, tag});
}
} // namespace mstl
