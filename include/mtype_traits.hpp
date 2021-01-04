#pragma once

namespace mstl {
// void_t maps any types into void.
// you can use it as a switch, if it is well formed
template <typename...> using void_t = void;

template <typename T> struct type_identity { using type = T; };

template <typename T, T v> struct integral_constant {
  static constexpr T value = v;
  using value_type = T;
  using type = integral_constant;
  constexpr operator value_type() const noexcept { return value; }

  // this is for serving integral_constant as a compile time callable.
  constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

} // namespace mstl

namespace mstl {
template <typename T> struct remove_reference { using type = T; };
template <typename T> struct remove_reference<T &> { using type = T; };
template <typename T> struct remove_reference<T &&> { using type = T; };

// try int overload fisrt, if failed fall back to the same type.
// if you want to add reference to a type, you need to be careful that
// void is handled correctly. voit & is illformed anyway.
template <typename T> auto try_add_lvalue_reference(int) -> type_identity<T &>;
template <typename T> auto try_add_lvalue_reference(...) -> type_identity<T>;

template <typename T> auto try_add_rvalue_reference(int) -> type_identity<T &&>;
template <typename T> auto try_add_rvalue_reference(...) -> type_identity<T>;

template <typename T>
struct add_lvalue_reference : decltype(mstl::try_add_lvalue_reference<T>(0)) {};

template <typename T>
struct add_rvalue_reference : decltype(mstl::try_add_rvalue_reference<T>(0)) {};

} // namespace mstl

namespace mstl {
// these are some pattern matching style templates
template <typename T> struct remove_cv { using type = T; };
template <typename T> struct remove_cv<const T> { using type = T; };
template <typename T> struct remove_cv<volatile T> { using type = T; };
template <typename T> struct remove_cv<const volatile T> { using type = T; };

template <typename T> struct remove_const { using type = T; };
template <typename T> struct remove_const<const T> { using type = T; };

template <typename T> struct remove_volatile { using type = T; };
template <typename T> struct remove_volatile<volatile T> { using type = T; };

template <typename T> struct remove_cvref {
  using type =
      typename mstl::remove_cv<typename mstl::remove_reference<T>::type>::type;
};

template <typename T> struct add_cv { using type = const volatile T; };
template <typename T> struct add_const { using type = const volatile T; };
template <typename T> struct add_volatile { using type = const volatile T; };

} // namespace mstl

namespace mstl {

// note there are cv pointers as well.
template <typename T> struct remove_pointer { using type = T; };
template <typename T> struct remove_pointer<T *> { using type = T; };
template <typename T> struct remove_pointer<T *const> { using type = T; };
template <typename T> struct remove_pointer<T *volatile> { using type = T; };
template <typename T> struct remove_pointer<T *const volatile> {
  using type = T;
};

} // namespace mstl

namespace mstl {
template <typename T, typename U> struct is_same : mstl::false_type {};
template <typename T> struct is_same<T, T> : mstl::true_type {};

template <typename T> struct is_lvalue_reference : mstl::false_type {};
template <typename T> struct is_lvalue_reference<T &> : mstl::true_type {};

template <typename T> struct is_rvalue_reference : mstl::false_type {};
template <typename T> struct is_rvalue_reference<T &&> : mstl::true_type {};

template <typename T> struct is_pointer : mstl::false_type {};
template <typename T> struct is_pointer<T *> : mstl::true_type {};

} // namespace mstl