// clang-format off
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "forwards.h"
#include "extension_points.h"

namespace pushmi {

template <class T>
using __category_t = typename T::category;

template <class T>
struct property_traits : property_traits<std::decay_t<T>> {
};
template <Decayed T>
struct property_traits<T> {
};
template <Decayed T>
  requires Valid<T, __category_t>
struct property_traits<T> {
  using category = __category_t<T>;
};

template <class T>
using category_t = __category_t<property_traits<T>>;

template<class T>
concept bool Property = Valid<T, category_t>;


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

template<class PropertySet0, class Category>
struct property_from_category;

template<class Property0, class... PropertyN, Property P>
struct property_from_category<property_set<Property0, PropertyN...>, P> {
  using type = typename property_from_category<property_set<Property0, PropertyN...>, category_t<P>>::type;
};

template<class Category, Property Property0, Property... PropertyN>
  requires !Property<Category>
struct property_from_category<property_set<Property0, PropertyN...>, Category> {
  using type = typename property_from_category<property_set<PropertyN...>, Category>::type;
};

template<Property Property0, Property... PropertyN>
struct property_from_category<property_set<Property0, PropertyN...>, category_t<Property0>> {
  using type = Property0;
};

template<class Category>
struct property_from_category<property_set<>, Category> {
};

template<class T, class... Set>
concept bool FoundIn = 
  requires(T&, Set&...) {
    (std::is_same<T, Set>::value || ... || false) == true;
  };

template<class... PropertyN>
concept bool UniqueCategory = 
  (Property<PropertyN> && ... && true) &&
  requires(PropertyN&...) {
    (FoundIn<category_t<PropertyN>, category_t<PropertyN>...> && ... && true) == true;
  };

template<class... PropertyN>
struct property_set : PropertyN... {
  static_assert((Property<PropertyN> && ... && true), "property_set only supports types that match the Property concept");
  static_assert(UniqueCategory<PropertyN...>, "property_set has multiple properties from the same category");

  // hide category in inherited types
  struct property_set_is_not_a_property;
  using category = property_set_is_not_a_property;

  template<class PropertySet>
  using insert_t = typename property_set_insert<property_set, PropertySet>::type;
  template<class Category>
  using from_category_t = typename property_from_category<property_set, Category>::type;
};

template<class T>
concept bool PropertySet = detail::is_v<T, property_set>;

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

template<Property... PropertyN, Property Expected>
constexpr bool PropertyQueryBase(const property_set<PropertyN...>&, const Expected&) { 
  return (Derived<PropertyN, Expected> || ... || false);
}
template<Property... PropertyN, Property... ExpectedN>
constexpr bool PropertySetQueryBase(const property_set<PropertyN...>& ps, const property_set<ExpectedN...>&) { 
  return (PropertyQueryBase(ps, ExpectedN{}) && ... && true);
}
template<Property... PropertyN, Property Expected>
constexpr bool PropertyCategoryQueryBase(const property_set<PropertyN...>&, const Expected&) { 
  return (Same<category_t<PropertyN>, category_t<Expected>> || ... || false);
}
template<Property... PropertyN, Property... ExpectedN>
constexpr bool PropertySetCategoryQueryBase(const property_set<PropertyN...>& ps, const property_set<ExpectedN...>&) { 
  return (PropertyCategoryQueryBase(ps, ExpectedN{}) && ... && true);
}

template<class PS, class... ExpectedN>
concept bool PropertyQuery = Properties<PS> && 
  (Property<ExpectedN> && ... && true) &&
  (PropertyQueryBase(properties_t<PS>{}, ExpectedN{}) && ... && true);

template<class PS, class Expected>
concept bool PropertySetQuery = Properties<PS> &&
  PropertySet<Expected> &&
  PropertySetQueryBase(properties_t<PS>{}, Expected{});

template<class PS, class... ExpectedN>
concept bool PropertyCategoryQuery = Properties<PS> &&
  (Property<ExpectedN> && ... && true) &&
  (PropertyCategoryQueryBase(properties_t<PS>{}, ExpectedN{}) && ... && true);

template<class PS, class Expected>
concept bool PropertySetCategoryQuery = Properties<PS> &&
  PropertySet<Expected> &&
  PropertySetCategoryQueryBase(properties_t<PS>{}, Expected{});


// tag types
struct cardinality_category {};
struct silent_tag { using category = cardinality_category;};
struct none_tag : silent_tag {};
struct single_tag : none_tag {};
struct many_tag : single_tag {};

struct flow_category {};
struct flow_tag {using category = flow_category;};

struct receiver_category {};
struct receiver_tag {using category = receiver_category;};

struct sender_category {};
struct sender_tag {using category = sender_category;};

struct time_tag : sender_tag {  };
struct constrained_tag : sender_tag {  };

// cardinality affects both sender and receiver

template <class PS>
concept bool Silent = PropertyQuery<PS, silent_tag>;

template <class PS>
concept bool None = Silent<PS> && PropertyQuery<PS, none_tag>;

template <class PS>
concept bool Single = None<PS> && PropertyQuery<PS, single_tag>;

template <class PS>
concept bool Many = Single<PS> && PropertyQuery<PS, many_tag>;

// flow affects both sender and receiver

template <class PS>
concept bool Flow = PropertyQuery<PS, flow_tag>;

// time and constrained are mutually exclusive (time is a special case of constrained and may be folded in later)

template <class PS>
concept bool Time = PropertyQuery<PS, time_tag>;

template <class PS>
concept bool Constrained = PropertyQuery<PS, constrained_tag>;

// sender and receiver are mutually exclusive

template <class PS>
concept bool Receiver = PropertyQuery<PS, receiver_tag> && Silent<PS>;

template <class PS>
concept bool Sender = PropertyQuery<PS, sender_tag> && Silent<PS>;




template <class S>
concept bool SilentReceiver = SemiMovable<S> && 
  Silent<S> && 
  Receiver<S> &&
  requires (S& s) {
    ::pushmi::set_done(s);
  };

template <class N, class E = std::exception_ptr>
concept bool NoneReceiver = SilentReceiver<N> &&
  None<N> &&
  SemiMovable<E> &&
  requires(N& n, E&& e) {
    ::pushmi::set_error(n, (E &&) e);
  };

template <class S, class T, class E = std::exception_ptr>
concept bool SingleReceiver = NoneReceiver<S, E> &&
  SemiMovable<T> &&
  SemiMovable<E> &&
  Single<S> &&
  requires(S& s, T&& t) {
    ::pushmi::set_value(s, (T &&) t); // Semantics: called exactly once.
  };

template <class S, class T, class E = std::exception_ptr>
concept bool ManyReceiver = SingleReceiver<S, T, E> &&
  SemiMovable<T> &&
  SemiMovable<E> &&
  Many<S>;




// silent does not really make sense, but cannot test for
// None without the error type, use none_tag to strengthen 
// requirements
template <class D>
concept bool SilentSender = SemiMovable<D> &&
  None<D> && 
  Sender<D>;

template <class D, class S>
concept bool SenderTo = SilentSender<D> &&
  SilentReceiver<S> &&
  requires(D& d, S&& s) {
    ::pushmi::submit(d, (S &&) s);
  };


// add concepts to support cancellation
//

template <class S>
concept bool FlowSilentSender = SilentSender<S> &&
  Flow<S>;

template <class S, class... PropertyN>
concept bool FlowSilentReceiver = SilentReceiver<S> &&
  Flow<S>;

template <class D, class S>
concept bool FlowSenderTo = FlowSilentSender<D> &&
  FlowSilentReceiver<S>;

template <
  class N, 
  class Up, 
  class PE = std::exception_ptr,
  class E = PE>
concept bool FlowNoneReceiver = FlowSilentReceiver<N> && 
  SilentReceiver<Up> &&
  SemiMovable<PE> &&
  SemiMovable<E> &&
  NoneReceiver<Up, PE> && 
  NoneReceiver<N, E> &&
  requires(N& n, Up&& up) {
    ::pushmi::set_stopping(n);
    ::pushmi::set_starting(n, (Up&&) up);
  };

template <
    class S,
    class Up,
    class T,
    class PE = std::exception_ptr,
    class E = PE>
concept bool FlowSingleReceiver = 
  SingleReceiver<S, T, E> && 
  FlowNoneReceiver<S, Up, PE, E>;

template <
    class S,
    class Up,
    class T,
    class PE = std::exception_ptr,
    class E = PE>
concept bool FlowManyReceiver = 
  ManyReceiver<S, T, E> && 
  FlowSingleReceiver<S, Up, T, PE, E>;


// add concepts for constraints
//

template <class D>
concept bool TimeSilentSender = SilentSender<D> && 
  Time<D> && 
  None<D> &&
  requires(D& d) {
    { ::pushmi::now(d) } -> Regular
  };

template <class D, class S>
concept bool TimeSenderTo = TimeSilentSender<D> && 
  SilentReceiver<S> &&
  requires(D& d, S&& s) {
    ::pushmi::submit(d, ::pushmi::now(d), (S &&) s);
  };

template <class D>
  requires TimeSilentSender<D>
using time_point_t = decltype(::pushmi::now(std::declval<D&>()));


// this is a more general form where the constraint could be time or priority
// enum or any other ordering constraint value-type.
//
// top() returns the constraint value that will cause the item to run asap.
// So now() for time and NORMAL for priority.
//
// I would like to replace Time.. with Constrained.. but not sure if it will
// obscure too much.

template <class D>
concept bool ConstrainedSilentSender = SilentSender<D> &&
  Constrained<D> &&
  None<D> && 
  requires(D& d) {
    { ::pushmi::top(d) } -> Regular
  };

template <class D, class S>
concept bool ConstrainedSenderTo = ConstrainedSilentSender<D> && 
  SilentReceiver<S> &&
  requires(D& d, S&& s) {
    ::pushmi::submit(d, ::pushmi::top(d), (S &&) s);
  };

template <class D>
  requires ConstrainedSilentSender<D>
using constraint_t = decltype(::pushmi::top(std::declval<D&>()));




// Mock concepts for use with the constrain() function template for
// constraining generic lambdas.
namespace mock {

template <class Out, class Tag = silent_tag>
struct Receiver {
  void operator()() requires pushmi::Receiver<Out, Tag> {}
};
template <class Out, class E = std::exception_ptr>
struct NoneReceiver {
  void operator()() requires pushmi::NoneReceiver<Out, E> {}
};
template <class Out, class V, class E = std::exception_ptr>
struct SingleReceiver {
  void operator()() requires pushmi::SingleReceiver<Out, V, E> {}
};

template <class D, class Tag = silent_tag>
struct Sender {
  void operator()() requires pushmi::Sender<D, Tag> {}
};
template <class D, class S, class Tag = silent_tag>
struct SenderTo {
  void operator()() requires pushmi::SenderTo<D, S, Tag> {}
};

template <class D, class Tag = silent_tag>
struct TimeSender {
  void operator()() requires pushmi::TimeSender<D, Tag> {}
};
template <class D, class S, class Tag = silent_tag>
struct TimeSenderTo {
  void operator()() requires pushmi::TimeSenderTo<D, S, Tag> {}
};

} // namespace mock

} // namespace pushmi
