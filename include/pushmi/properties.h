#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <type_traits>

namespace pushmi {

// property_set implements a map of category-type to property-type.
// for each category only one property in that category is allowed in the set.

// customization point for a property with a category

template <class T>
using __property_category_t = typename T::property_category;

template <class T>
struct property_traits : property_traits<std::decay_t<T>> {
};
template <Decayed T>
struct property_traits<T> {
};
template <Decayed T>
  requires Valid<T, __property_category_t>
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
    ((std::is_same<T, Set>::value ? 1 : 0) + ... + 0) == 1;
  };

template<class... PropertyN>
concept bool UniqueCategory = 
  (Property<PropertyN> && ... && true) &&
  requires(PropertyN&...) {
    (FoundExactlyOnce<property_category_t<PropertyN>, property_category_t<PropertyN>...> && ... && true) == true;
  };



template<class... PropertyN>
struct property_set {
  static_assert((Property<PropertyN> && ... && true), "property_set only supports types that match the Property concept");
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
template <Decayed T>
struct property_set_traits<T> {
};
template <Decayed T>
  requires Valid<T, __properties_t>
struct property_set_traits<T> {
  using properties = __properties_t<T>;
};

template <class T>
  requires PropertySet<__properties_t<property_set_traits<T>>>
using properties_t = __properties_t<property_set_traits<T>>;

template<class T>
concept bool Properties = Valid<T, properties_t>;

// insert
// insert will replace the property in the left set with the property in the 
// right set that matches on the category-type and add the properties from the 
// right set that do not match on the category-type of any of the properties
// in the left set.

template<class PropertySet0, class... PropertySetN>
struct property_set_insert;

template<class... Properties0, class Property0, class... PropertyN>
  requires Property<Property0>
struct property_set_insert<property_set<Properties0...>, Property0, PropertyN...> {
  using type = typename property_set_insert<property_set<Properties0..., Properties0...>, PropertyN...>::type;
};

template<class... Properties0, class... Properties1, class... PropertySetN>
struct property_set_insert<property_set<Properties0...>, property_set<Properties1...>, PropertySetN...> {
  using type = typename property_set_insert<property_set<Properties0..., Properties1...>, PropertySetN...>::type;
};

template<class... Properties0, class Property, class... PropertySetN>
struct property_set_insert<property_set<Properties0...>, Property, PropertySetN...> {
  using type = typename property_set_insert<property_set<Properties0..., Property>, PropertySetN...>::type;
};

template<class... Properties0>
struct property_set_insert<property_set<Properties0...>> {
  using type = property_set<Properties0...>;
};

template<class PS0, class PS1>
using property_insert_t = typename property_set_insert<PS0, PS1>::type;

// find property in the specified set that matches the category of the property specified.

template<class PropertySet0, class Category>
struct property_from_category;

template<class Property0, class... PropertyN, Property P>
struct property_from_category<property_set<Property0, PropertyN...>, P> {
  using type = typename property_from_category<property_set<Property0, PropertyN...>, property_category_t<P>>::type;
};

template<class Category, Property Property0, Property... PropertyN>
  requires !Property<Category>
struct property_from_category<property_set<Property0, PropertyN...>, Category> {
  using type = typename property_from_category<property_set<PropertyN...>, Category>::type;
};

template<Property Property0, Property... PropertyN>
struct property_from_category<property_set<Property0, PropertyN...>, property_category_t<Property0>> {
  using type = Property0;
};

template<class Category>
struct property_from_category<property_set<>, Category> {
};

template<class PS, class C>
using property_from_category_t = typename property_from_category<PS, C>::type;


// query for properties on types with properties.

template<class Expected, class... TN>
struct found_base : 
  std::integral_constant<bool, (std::is_base_of<Expected, TN>::value || ... || false)> {};
template<class Expected, class... TN>
constexpr bool found_base_v = found_base<Expected, TN...>::value;

template<class PS, class... ExpectedN>
struct property_query : std::false_type {};

template<class PS, class... ExpectedN>
  requires Properties<PS>
struct property_query<PS, ExpectedN...> : property_query<properties_t<PS>, ExpectedN...> {};

template<class... PropertyN, class... ExpectedN>
  requires (Property<PropertyN> && ... && true) &&
  (Property<ExpectedN> && ... && true)
struct property_query<property_set<PropertyN...>, ExpectedN...> : 
  std::integral_constant<bool, (found_base_v<ExpectedN, PropertyN...> && ... && true)> {};

template<class PS, class... ExpectedN>
inline constexpr bool property_query_v = property_query<PS, ExpectedN...>::value;

} // namespace pushmi
