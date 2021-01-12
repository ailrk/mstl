#pragma once
#include "mtype_traits.hpp"
#include "utility.hpp"

namespace mstl {
// get the real address of object.
// This is the equivalence of &, and it can be used when & is
// overloaded.
template <typename T> constexpr T *addressof(T &arg) noexcept {
  return reinterpret_cast<T *>(
      &const_cast<char &>(reinterpret_cast<const volatile char &>(arg)));
}

template <typename T>
constexpr T *addressof(const T &&arg) = delete; // no addr for const rvalue

} // namespace mstl

namespace mstl {
template <typename T> class allocator;

template <> class allocator<void> {
  using pointer = void *;
  using const_pointer = const void *;
  using value_type = void;

  template <typename U> struct rebind { using other = allocator<U>; };
};

template <> class allocator<const void> {
  using pointer = const void *;
  using const_pointer = const void *;
  using value_type = const void;

  template <typename U> struct rebind { using other = allocator<U>; };
};

} // namespace mstl

namespace mstl { // poiner traits

namespace pointer_traits_UTILL {
// some pointer traits helpers to check if certain member exists.
// we need this to check for arbitrary types.

template <typename T, typename = void>
struct has_element_type__ : mstl::false_type {}; // base
// technique to check if something is well formed or illformed.
// use the type as the argument of voit_t, which maps all types to void.
// if it's well formed, this template will specialized to void,
// if it's ill formed, compiler will stop.
template <typename T>
struct has_element_type__<T, mstl::void_t<typename T::element_type>>
    : mstl::true_type {};
template <typename Ptr, bool = has_element_type__<Ptr>::value>

struct pointer_trait_element_type__; // base
template <typename Ptr> struct pointer_trait_element_type__<Ptr, true> {
  using type = typename Ptr::element_type;
};
// without varadic template you need to write lots of overloads...
template <template <typename, typename...> typename Sp, typename Tp,
          typename... Args>
struct pointer_trait_element_type__<Sp<Tp, Args...>, true> {
  using type = typename Sp<Tp, Args...>::element_type;
};
template <template <typename, typename...> typename Sp, typename Tp,
          typename... Args>
struct pointer_trait_element_type__<Sp<Tp, Args...>, false> {
  using type = Tp;
};

// Function overloading at the type level.
// Fancy sfinae technique with test function overloading.
// the type two__ is basically a false type, while char is true type.
// if the type returned by test__ has size 1, then we know the char
// case is triggered, otherwise two__ case is triggered.
// Choose char instead of other types is because the size of char is
// the same for all implementation, we can expect stable behavior.
//
// In our case, if 0 is passed in, template will try to deduce the type.
// for test__ we have two cases, the first one takes any type, the second
// one is more specific, takes type that contains rebind<U>.
// If X has rebind, the second one will be the one to specialize, and we
// get the overload that return char.
template <typename T, typename U> struct has_rebind__ {
private:
  struct two__ {
    char lx__;
    char lxx__;
  };
  template <typename X> static two__ test__(...);
  template <typename X>
  static char test__(typename X::template rebind<U> * = 0);

public:
  static const bool value = sizeof(test__<T>(0)) == 1;
};

template <typename T, typename U, bool = has_rebind__<T, U>::value>
struct pointer_traits_rebind__ {
  using type = typename T::template rebind<U>::other;
};

// the logic is if Tp has rebind, then use the rebind from Tp. In case
// Tp doesn't have rebind, Mannually rebind Tp in Sp<Tp, Rrgs...> to U.
template <template <typename, typename...> typename Sp, typename Tp,
          typename... Args, typename U>
struct pointer_traits_rebind__<Sp<Tp, Args...>, U, true> {
  using type = typename Tp::template rebind<U>::other;
};

template <template <typename, typename...> typename Sp, typename Tp,
          typename... Args, typename U>
struct pointer_traits_rebind__<Sp<Tp, Args...>, U, false> {
  using type = Sp<U, Args...>;
};

template <typename T, typename = void>
struct has_difference_type__ : mstl::false_type {};

template <typename T>
struct has_difference_type__<T, mstl::void_t<typename T::difference_type>>
    : mstl::true_type {};

template <typename Ptr, bool = has_difference_type__<Ptr>::value>
struct pointer_trait_difference_type {
  using type = ptrdiff_t;
};
template <typename Ptr> struct pointer_trait_difference_type<Ptr, true> {
  using type = typename Ptr::difference_type;
};

} // namespace pointer_traits_UTILL

// Not all pointers are in form T*. For example, when you have two processes
// own pointers point to the same location in a shared memory, their address
// might be different in each of their virtual address space. A solution from
// boost is to create a struct offset_ptr that maintain the offset between
// the pointer and the object it points to.
// Clearly offset_ptr is not a T*, although we want to treat it as one.
// The goal of pointer_traits is to provide an uniform interface for these
// pointer types,
//
// More to read:
// http://blog.nuggetwheat.org/index.php/2015/09/01/why-pointer_traits-was-introduced-in-c11/
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0653r0.html
template <typename Ptr> struct pointer_traits {
  using pointer = Ptr;
  using element_type =
      typename pointer_traits_UTILL::pointer_trait_element_type__<Ptr>::type;
  using difference_type =
      typename pointer_traits_UTILL::pointer_trait_difference_type<Ptr>::type;

  template <typename U> struct rebind {
    using other =
        typename pointer_traits_UTILL::pointer_traits_rebind__<Ptr, U>::type;
  };

private:
  struct nat__ {};

public:
  static pointer
  pointer_to(typename mstl::conditional<mstl::is_void<element_type>::value,
                                        nat__, element_type>::type &r__) {
    return pointer::pointer_to(r__);
  }
};

// Note the class above doesn't handle raw pointer directly, so we
// need overload for the T* case. Because it's just raw pointer, it's pretty
// easy to implement.
template <typename T> struct pointer_traits<T *> {
  using pointer = T *;
  using element_type = T;
  using difference_type = ptrdiff_t;

  template <typename U> struct rebind { using Other = U *; };

private:
  struct nat__ {};

public:
  // here, if we use &r__, it's possible that r__ overload & and
  // bahave completely different from what we intend it to.
  static pointer pointer_to(
      typename mstl::conditional<mstl::is_void<element_type>::value, nat__,
                                 element_type>::type &r__) noexcept {
    return mstl::addressof(r__);
  }
};

} // namespace mstl

namespace mstl::memory_UTIL {

// obtain true raw pointer from any pointer types.
template <typename T> inline T *to_raw_pointer__(T *p) noexcept { return p; }

// if it's a pointer like type, use operator->() to obtain the real
// pointer.
template <typename Ptr>
inline typename mstl::pointer_traits<Ptr>::element_type *
to_raw_pointer__(Ptr p) noexcept {
  return to_raw_pointer__(p.operator->());
}

// NOTE: c++20 move to_address into pointer_traits, using c++17 implementation
// for now.
} // namespace mstl::memory_UTIL

namespace mstl {

// to_address takes fancy pointer and return the raw pointer for it's
// element_type
template <typename T> constexpr T *to_address(T *p) noexcept {
  static_assert(!mstl::is_function<T>::value, "T is a function type");
  return p;
}

template <typename Ptr> auto to_address(const Ptr &p) noexcept {
  return mstl::memory_UTIL::to_raw_pointer__(p);
}
} // namespace mstl

namespace mstl // allocator traits
{}             // namespace mstl

namespace mstl {

// only for unique pointer
template <typename T> struct default_delete {
  constexpr default_delete() noexcept = default;
  template <typename U> default_delete(const default_delete<U> &) noexcept;
  void operator()(T *) const noexcept;
};

template <typename T> struct default_delete<T[]> {
  constexpr default_delete() noexcept = default;
  void operator()(T *) const noexcept;
  template <typename U> void operator()(U *) const = delete;
};

template <typename T, typename D = default_delete<T>> class unique_ptr {
public:
  using element_type = T;
  using deleter_type = D;
  using pointer = T *; // if default_delete::pointer present, use it. TODO

  constexpr unique_ptr() noexcept;
  explicit unique_ptr(pointer p) noexcept;

  unique_ptr(unique_ptr &&u) noexcept;
  unique_ptr(std::nullptr_t) noexcept : unique_ptr() {}

  template <typename U, typename E> unique_ptr(unique_ptr<U, E> &&u) noexcept;
  template <typename U> unique_ptr(unique_ptr<U> &&u) noexcept;

  ~unique_ptr();

  typename mstl::add_lvalue_reference<T>::type operator*() const;
  pointer operator->() const noexcept;
  pointer get() const noexcept;
  deleter_type &get_deleter() noexcept;
  const deleter_type &get_deleter() const noexcept;
  explicit operator bool() const noexcept;

  // modifier

  pointer release() noexcept;
  void reset(pointer p = pointer()) noexcept;
  void swap(unique_ptr &u) noexcept;
};

template <typename T, typename D>
void swap(mstl::unique_ptr<T, D> &x, mstl::unique_ptr<T, D> &y) noexcept;

template <typename T1, typename D1, typename T2, typename D2>
bool operator==(const mstl::unique_ptr<T1, D1> &x,
                const mstl::unique_ptr<T2, D2> &y);

template <typename T1, typename D1, typename T2, typename D2>
bool operator!=(const mstl::unique_ptr<T1, D1> &x,
                const mstl::unique_ptr<T2, D2> &y) {
  return !(x == y);
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator<(const mstl::unique_ptr<T1, D1> &x,
               const mstl::unique_ptr<T2, D2> &y);

template <typename T1, typename D1, typename T2, typename D2>
bool operator<=(const mstl::unique_ptr<T1, D1> &x,
                const mstl::unique_ptr<T2, D2> &y);

template <typename T1, typename D1, typename T2, typename D2>
bool operator>(const mstl::unique_ptr<T1, D1> &x,
               const mstl::unique_ptr<T2, D2> &y);

template <typename T1, typename D1, typename T2, typename D2>
bool operator>=(const mstl::unique_ptr<T1, D1> &x,
                const mstl::unique_ptr<T2, D2> &y);

} // namespace mstl
