#pragma once

#include <cstddef>
#include <type_traits>

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

template <bool Pred, typename If, typename Else> struct conditional;

template <typename If, typename Else> struct conditional<true, If, Else> {
  using type = If;
};

template <typename If, typename Else> struct conditional<false, If, Else> {
  using type = Else;
};

} // namespace mstl

namespace mstl {

// the source of all evil.
template <bool B, typename T = void> struct enable_if {};
template <typename T> struct enable_if<true, T> { using type = T; };

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

// there are some type traits require compiler intrinsics.
namespace mstl {} // namespace mstl

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
// There are some type traits can only be implemented with compiler intrinsics.
// I don't bother with those.
template <typename T> struct is_enum : std::is_enum<T> {};
template <typename T> struct is_union : std::is_union<T> {};

} /* namespace mstl */

namespace mstl {
template <typename T, typename U> struct is_same : mstl::false_type {};
template <typename T> struct is_same<T, T> : mstl::true_type {};

template <typename T> struct is_lvalue_reference : mstl::false_type {};
template <typename T> struct is_lvalue_reference<T &> : mstl::true_type {};

template <typename T> struct is_rvalue_reference : mstl::false_type {};
template <typename T> struct is_rvalue_reference<T &&> : mstl::true_type {};

template <typename T>
struct is_void : mstl::is_same<typename mstl::remove_cv<T>::type, void> {};

template <typename T>
struct is_null_pointer
    : mstl::is_same<typename mstl::remove_cv<T>::type, std::nullptr_t> {};

template <typename T>
struct is_integral
    : mstl::integral_constant<
          bool, mstl::is_same<typename mstl::remove_cv<T>::type, int>::value ||
                    mstl::is_same<typename mstl::remove_cv<short>::type,
                                  short>::value ||
                    mstl::is_same<typename mstl::remove_cv<bool>::type,
                                  bool>::value ||
                    mstl::is_same<typename mstl::remove_cv<char>::type,
                                  char>::value ||
                    mstl::is_same<typename mstl::remove_cv<char16_t>::type,
                                  char16_t>::value ||
                    mstl::is_same<typename mstl::remove_cv<char32_t>::type,
                                  char32_t>::value ||
                    mstl::is_same<typename mstl::remove_cv<wchar_t>::type,
                                  wchar_t>::value ||
                    mstl::is_same<typename mstl::remove_cv<long>::type,
                                  long>::value ||
                    mstl::is_same<typename mstl::remove_cv<long long>::type,
                                  long long>::value> {};

template <typename T>
struct is_floating_point
    : mstl::integral_constant<
          bool,
          mstl::is_same<typename mstl::remove_cv<T>::type, float>::value ||
              mstl::is_same<typename mstl::remove_cv<T>::type, double>::value ||
              mstl::is_same<typename mstl::remove_cv<T>::type,
                            long double>::value> {};

template <typename T>
struct is_arithmetic
    : mstl::integral_constant<bool, mstl::is_integral<T>::value ||
                                        mstl::is_floating_point<T>::value> {};

template <typename T> struct is_pointer_ : mstl::false_type {};
template <typename T> struct is_pointer_<T *> : mstl::true_type {};
template <typename T>
struct is_pointer : is_pointer_<typename mstl::remove_cv<T>::type> {};

template <typename T> struct is_array : mstl::false_type {};
template <typename T> struct is_array<T[]> : mstl::true_type {};
template <typename T, size_t N> struct is_array<T[N]> : mstl::true_type {};

template <typename T> struct is_member_pointer_ : mstl::false_type {};
template <typename T, typename U>
struct is_member_pointer_<T U::*> : mstl::true_type {};

template <typename T>
struct is_member_pointer
    : is_member_pointer_<typename mstl::remove_cv<T>::type> {};

// fundamental types like arithmetics, void, nullptr
template <typename T>
struct is_fundamental
    : mstl::integral_constant<
          bool, mstl::is_arithmetic<T>::value || mstl::is_void<T>::value ||
                    mstl::is_same<std::nullptr_t,
                                  typename mstl::remove_cv<T>::type>::value> {};

// a compound type is a type that is constructed from fundamental types
template <typename T>
struct is_compound
    : mstl::integral_constant<bool, !mstl::is_fundamental<T>::value> {};

// check if it's a class by checking if member pointers exist.
// here we faked a member pointer int T::* up. The class doesn't need to
// have a int member at all. As long as the type is a class, this type is valid.
// On the other hand, any other type can't have this definition.
template <typename T, typename = void> struct is_class : false_type {};
template <typename T>
struct is_class<T, typename mstl::void_t<int T::*>> : true_type {};

template <typename T> struct is_reference : false_type {};
template <typename T> struct is_reference<T &> : true_type {};
template <typename T> struct is_reference<T &&> : true_type {};

// not array or class.
template <typename T>
struct is_scalar
    : mstl::integral_constant<bool, mstl::is_arithmetic<T>::value ||
                                        mstl::is_enum<T>::value ||
                                        mstl::is_pointer<T>::value ||
                                        mstl::is_member_pointer<T>::value ||
                                        mstl::is_null_pointer<T>::value> {};

// not function, reference, or void
template <typename T>
struct is_object
    : mstl::integral_constant<
          bool, mstl::is_array<T>::value || mstl::is_scalar<T>::value ||
                    mstl::is_union<T>::value || mstl::is_class<T>::value> {};

template <typename T> struct is_const : mstl::false_type {};
template <typename T> struct is_const<const T> : mstl::true_type {};

} // namespace mstl

namespace mstl {
template <typename T> struct is_function : mstl::false_type {};

// regular function
template <typename Ret, class... Args>
struct is_function<Ret(Args...)> : mstl::true_type {};

// varaidc functions like printf
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...)> : mstl::true_type {};

// bunch of cv qualifiers...
template <typename Ret, class... Args>
struct is_function<Ret(Args...) const> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) volatile> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : mstl::true_type {};

template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) const> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile> : mstl::true_type {};

// then bunch of ref qualfiers...
template <typename Ret, class... Args>
struct is_function<Ret(Args...) &> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) const &> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) volatile &> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) const volatile &> : mstl::true_type {};

template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) &> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) const &> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile &> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile &> : mstl::true_type {};

template <typename Ret, class... Args>
struct is_function<Ret(Args...) &&> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) const &&> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) volatile &&> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&> : mstl::true_type {};

template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) &&> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) const &&> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile &&> : mstl::true_type {};
template <typename Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile &&> : mstl::true_type {};
} // namespace mstl
