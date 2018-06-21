#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "traits.h"

namespace pushmi {

// property_set implements a map of category-type to property-type.
// for each category only one property in that category is allowed in the set.

// customization point for a property with a category

template <class T>
using __property_category_t = typename T::property_category;

template <class T>
struct property_traits : property_traits<std::decay_t<T>> {
};
template <class T>
  requires Decayed<T>
struct property_traits<T> {
};
template <class T>
  requires Decayed<T> && Valid<T, __property_category_t>
struct property_traits<T> {
  using property_category = __property_category_t<T>;
};

template <class T>
using property_category_t = __property_category_t<property_traits<T>>;

template<class T>
concept bool Property = Valid<T, property_category_t>;


// in cases where Set contains T, allow T to find itself only once
template<class T, class... Set>
concept bool FoundExactlyOnce = 
  requires(T&, Set&...) {
    sum_v<(std::is_same<T, Set>::value ? 1 : 0)...> == 1;
  };

template<class... PropertyN>
concept bool UniqueCategory = 
  And<Property<PropertyN>...> &&
  requires(PropertyN&...) {
    all_true_v<FoundExactlyOnce<property_category_t<PropertyN>, property_category_t<PropertyN>...>...>;
  };



template<class... PropertyN>
struct property_set {
  static_assert(all_true_v<Property<PropertyN>...>, "property_set only supports types that match the Property concept");
  static_assert(UniqueCategory<PropertyN...>, "property_set has multiple properties from the same category");
};



template<class T>
concept bool PropertySet = detail::is_v<T, property_set>;

// customization point for a type with properties

template <class T>
using __properties_t = typename T::properties;

template <class T>
struct property_set_traits : property_traits<std::decay_t<T>> {
};
template <class T>
  requires Decayed<T>
struct property_set_traits<T> {
};
template <class T>
  requires Decayed<T> && Valid<T, __properties_t>
struct property_set_traits<T> {
  using properties = __properties_t<T>;
};

template <class T>
  requires PropertySet<__properties_t<property_set_traits<T>>>
using properties_t = __properties_t<property_set_traits<T>>;

template<class T>
concept bool Properties = Valid<T, properties_t>;

// find property in the specified set that matches the category of the property specified.

template<class... TN>
struct property_from_category;

template<class PS, class P>
  requires Properties<PS> && Property<P>
struct property_from_category<PS, P> : property_from_category<properties_t<PS>, property_category_t<P>> {};

template<class... PN, class Category>
  requires And<Property<PN>...> && !Property<Category>
struct property_from_category<property_set<PN...>, Category> : property_from_category<PN, property_category_t<PN>, Category>... {};

template<class P, class Category>
  requires Property<P> && !Property<Category>
struct property_from_category<P, Category, Category> {
  using type = P;
};
template<class P, class Category, class Expected>
  requires Property<P> && !Property<Category> && !Property<Expected>
struct property_from_category<P, Category, Expected> {};

template<class PS, class C>
using property_from_category_t = typename property_from_category<PS, C>::type;

// remove property in the specified set that matches the category of the property specified.

template<class... TN>
struct remove_property_from_category;

template<class PS, class P>
  requires Properties<PS> && Property<P>
struct remove_property_from_category<PS, P> : remove_property_from_category<properties_t<PS>, property_category_t<P>> {};

template<class P0, class... PN, class Category>
  requires Property<P0> && And<Property<PN>...> && !Property<Category>
struct remove_property_from_category<property_set<P0, PN...>, Category> {
  using type = typename remove_property_from_category<property_set<>, Category, P0, PN...>::type;
};

template<class Category>
  requires !Property<Category>
struct remove_property_from_category<property_set<>, Category> {
  using type = property_set<>;
};

template<class... PN, class Category, class R0, class... RN>
  requires And<Property<PN>...> && !Property<Category> && !Same<property_category_t<R0>, Category>
struct remove_property_from_category<property_set<PN...>, Category, R0, RN...> {
  using type = typename remove_property_from_category<property_set<PN..., R0>, Category, RN...>::type;
};

template<class... PN, class Category, class R0, class... RN>
  requires And<Property<PN>...> && !Property<Category> && Same<property_category_t<R0>, Category>
struct remove_property_from_category<property_set<PN...>, Category, R0, RN...> {
  using type = typename remove_property_from_category<property_set<PN...>, Category, RN...>::type;
};

template<class... PN, class Category, class R0>
  requires And<Property<PN>...> && !Property<Category> && !Same<property_category_t<R0>, Category>
struct remove_property_from_category<property_set<PN...>, Category, R0> {
  using type = property_set<PN..., R0>;
};

template<class... PN, class Category, class R0>
  requires And<Property<PN>...> && !Property<Category> && Same<property_category_t<R0>, Category>
struct remove_property_from_category<property_set<PN...>, Category, R0> {
  using type = property_set<PN...>;
};

template<class PS, class C>
using remove_property_from_category_t = typename remove_property_from_category<PS, C>::type;

// insert
// insert will replace the property in the left set with the property in the 
// right set that matches on the category-type and add the properties from the 
// right set that do not match on the category-type of any of the properties
// in the left set.

template<class PropertySet0, class... PropertySetN>
struct property_set_insert;

template<class... Properties0, class Property0, class... PropertyN>
  requires Property<Property0> && And<!Same<property_category_t<Properties0>, property_category_t<Property0>>...>
struct property_set_insert<property_set<Properties0...>, Property0, PropertyN...> {
  using type = typename property_set_insert<property_set<Properties0..., Property0>, PropertyN...>::type;
};

template<class... Properties0, class Property0, class... PropertyN>
  requires Property<Property0> && Or<Same<property_category_t<Properties0>, property_category_t<Property0>>...>
struct property_set_insert<property_set<Properties0...>, Property0, PropertyN...> {
  using type = typename property_set_insert<remove_property_from_category_t<property_set<Properties0...>, property_category_t<Property0>>, Property0, PropertyN...>::type;
};

template<class... Properties0, class... Properties1, class... PropertySetN>
struct property_set_insert<property_set<Properties0...>, property_set<Properties1...>, PropertySetN...> {
  using type = typename property_set_insert<property_set<Properties0...>, Properties1..., PropertySetN...>::type;
};

template<class... Properties0>
struct property_set_insert<property_set<Properties0...>> {
  using type = property_set<Properties0...>;
};

template<class PS0, class PS1>
using property_insert_t = typename property_set_insert<PS0, PS1>::type;

// query for properties on types with properties.

template<class Expected, class... TN>
struct found_base : 
  std::integral_constant<bool, any_true_v<std::is_base_of<Expected, TN>::value...>> {};
template<class Expected, class... TN>
constexpr bool found_base_v = found_base<Expected, TN...>::value;

template<class PS, class... ExpectedN>
struct property_query : std::false_type {};

template<class PS, class... ExpectedN>
  requires Properties<PS>
struct property_query<PS, ExpectedN...> : property_query<properties_t<PS>, ExpectedN...> {};

template<class... PropertyN, class... ExpectedN>
  requires And<Property<PropertyN>...> &&
  And<Property<ExpectedN>...>
struct property_query<property_set<PropertyN...>, ExpectedN...> : 
  std::integral_constant<bool, all_true_v<found_base_v<ExpectedN, PropertyN...>...>> {};

template<class PS, class... ExpectedN>
PUSHMI_INLINE_VAR constexpr bool property_query_v = property_query<PS, ExpectedN...>::value;

} // namespace pushmi
