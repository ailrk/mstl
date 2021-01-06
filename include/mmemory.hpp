#pragma once
#include "mtype_traits.hpp"
#include "utility.hpp"

namespace mstl {

// get the real address of object.
template <typename T> constexpr T *addressof(T &arg) noexcept {
  return reinterpret_cast<T *>(
      &const_cast<char &>(reinterpret_cast<const volatile char &>(arg)));
}
template <typename T>
constexpr T *addressof(const T &&arg) = delete; // no addr for const rvalue

} // namespace mstl

namespace mstl {
// a standarized way to access pointer like types.
// iterator can be accessed by a pointer trait.
template <typename Ptr> struct pointer_traits {
  using pointer = Ptr;
  using element_type = typename mstl::remove_pointer<Ptr>::type;
  using difference_type = size_t;

  template <typename U> using rebind = void;
};

template <typename T> struct pointer_traits<T *> {
  using pointer = T *;
  using element_type = T;
  using difference_type = ptrdiff_t;

  template <typename U> using rebind = U *;


};

} // namespace mstl

namespace mstl {

//
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
