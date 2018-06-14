#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <functional>
#include <type_traits>

#include <meta/meta.hpp>

namespace pushmi {
namespace detail {
  template <bool...>
  struct bools;
}

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T, template <class> class C>
concept bool Valid = requires { typename C<T>; };

template <class T, template<class...> class Trait, class... Args>
concept bool Satisfies = bool(Trait<T>::type::value);

template <class T, class U>
concept bool Same = __is_same_as(T, U) && __is_same_as(U, T);

#if __cpp_fold_expressions >= 201603
template <bool...Bs>
concept bool And = (Bs &&...);
#else
template <bool...Bs>
concept bool And = Same<detail::bools<Bs..., true>, detail::bools<true, Bs...>>;
#endif

template <class T>
concept bool Object = requires(T* p) {
  *p;
  { p } -> const volatile void*;
};

template <class T, class... Args>
concept bool Constructible = __is_constructible(T, Args...);

template <class T>
concept bool MoveConstructible = Constructible<T, T>;

template <class From, class To>
concept bool ConvertibleTo =
    std::is_convertible<From, To>::value && requires(From (&f)()) {
  static_cast<To>(f());
};

template <class A, class B>
concept bool Derived = __is_base_of(B, A);

template <class A>
concept bool Decayed = Same<A, std::decay_t<A>>;

template <class T, class U>
concept bool Assignable = Same<T, T&>&& requires(T t, U&& u) {
  { t = (U &&) u } -> Same<T>&&;
};

template <class T>
concept bool EqualityComparable = requires(remove_cvref_t<T> const & t) {
  { t == t } -> bool;
  { t != t } -> bool;
};

template <class T>
concept bool SemiMovable =
    Object<T>&& Constructible<T, T>&& ConvertibleTo<T, T>;

template <class T>
concept bool Movable = SemiMovable<T>&& Assignable<T&, T>;

template <class T>
concept bool Copyable = Movable<T>&&
  Assignable<T&, const T&> &&
  ConvertibleTo<const T&, T>;

template <class T>
concept bool Semiregular = Copyable<T>&& Constructible<T>;

template <class T>
concept bool Regular = Semiregular<T>&& EqualityComparable<T>;

#if __cpp_lib_invoke >= 201411
using std::invoke;
using std::invoke_result;
using std::invoke_result_t;
#else
template <class F, class...As>
  requires requires (F&& f, As&&...as) { ((F&&) f)((As&&) as...); }
decltype(auto) invoke(F&& f, As&&...as)
    noexcept(noexcept(((F&&) f)((As&&) as...))) {
  return ((F&&) f)((As&&) as...);
}
template <Satisfies<std::is_member_pointer> F, class...As>
  requires requires (F f, As&&...as) { std::mem_fn(f)((As&&) as...); }
decltype(auto) invoke(F f, As&&...as)
    noexcept(noexcept(std::mem_fn(f)((As&&) as...))) {
  return std::mem_fn(f)((As&&) as...);
}
template <class F, class...As>
using invoke_result_t =
  decltype(pushmi::invoke(std::declval<F>(), std::declval<As>()...));
template <class F, class...As>
struct invoke_result : meta::defer<invoke_result_t, F, As...> {};
#endif

template <class F, class... Args>
concept bool Invocable = requires(F&& f, Args&&... args) {
  pushmi::invoke((F &&) f, (Args &&) args...);
};

template <class F, class... Args>
concept bool NothrowInvocable =
    Invocable<F, Args...> && requires(F&& f, Args&&... args) {
  { pushmi::invoke((F &&) f, (Args &&) args...) } noexcept;
};

namespace detail {
// is_ taken from meta library

template <typename, template <typename...> class>
struct is_ : std::false_type {};

template <typename... Ts, template <typename...> class C>
struct is_<C<Ts...>, C> : std::true_type {};

template <typename T, template <typename...> class C>
constexpr bool is_v = is_<T, C>::value;

template <bool B, class T = void>
using requires_ = std::enable_if_t<B, T>;

} // namespace detail

namespace mock {
template <class F, class... Args>
struct Invocable {
  void operator()() requires pushmi::Invocable<F, Args...> {}
};
} // namespace mock

} // namespace pushmi
