#pragma once
#include "mtype_traits.hpp"
#include "utility.hpp"

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

namespace mstl {

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
template <typename Ptr, bool = mstl::has_element_type__<Ptr>::value>

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
//
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

template <typename T, typename = void>
struct has_difference_type__ : mstl::false_type {};

template <typename T>
struct has_difference_type__<T, mstl::void_t<typename T::difference_type>>
    : mstl::true_type {};

template <typename Ptr, bool = mstl::has_difference_type__<Ptr>::value>
struct pointer_trait_difference_type {
  using type = ptrdiff_t;
};
template <typename Ptr> struct pointer_trait_difference_type<Ptr, true> {
  using type = typename Ptr::difference_type;
};

// get the real address of object.
template <typename T> constexpr T *addressof(T &arg) noexcept {
  return reinterpret_cast<T *>(
      &const_cast<char &>(reinterpret_cast<const volatile char &>(arg)));
}

template <typename T>
constexpr T *addressof(const T &&arg) = delete; // no addr for const rvalue

// a standarized way to access pointer like types.
// iterator can be accessed by a pointer trait.
template <typename Ptr> struct pointer_traits {
  using pointer = Ptr;
  using element_type = typename mstl::remove_pointer<Ptr>::type;
  using difference_type = size_t;

  template <typename U> using rebind = void;
};

// of course pointer_traits works with raw pointer too.
template <typename T> struct pointer_traits<T *> {
  using pointer = T *;
  using element_type = T;
  using difference_type = ptrdiff_t;

  template <typename U> using rebind = U *;
};

template <typename T> constexpr T *to_address(T *p) noexcept {
  static_assert(!mstl::is_function<T>::value, "T is a function type");
  return p;
}
template <typename Ptr> inline auto to_address(const Ptr &p) noexcept {}

} // namespace mstl

namespace mstl {

template <typename T> inline T *to_raw_pointer__(T *p) noexcept { return p; }

template <typename Ptr>
inline typename mstl::pointer_traits<Ptr>::element_type *
to_raw_pointer__(Ptr p) noexcept {
  return mstl::to_raw_pointer__(p);
}

} // namespace mstl

namespace mstl {} // namespace mstl

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
