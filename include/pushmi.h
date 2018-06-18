#pragma once
#ifndef PUSHMI_SINGLE_HEADER
#define PUSHMI_SINGLE_HEADER

// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <cstddef>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <chrono>
#include <exception>
#include <functional>
#include <algorithm>
#include <utility>
#include <memory>
#include <type_traits>
#include <initializer_list>

#include <thread>
#include <future>
#include <tuple>
#include <deque>

#if __cpp_lib_optional >= 201606
#include <optional>
#endif
/// \file meta_fwd.hpp Forward declarations
//
// Meta library
//
//  Copyright Eric Niebler 2014-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_FWD_HPP
#define META_FWD_HPP

//#include <utility>

#ifndef META_DISABLE_DEPRECATED_WARNINGS
#ifdef __cpp_attribute_deprecated
#define META_DEPRECATED(MSG) [[deprecated(MSG)]]
#else
#if defined(__clang__) || defined(__GNUC__)
#define META_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#elif defined(_MSC_VER)
#define META_DEPRECATED(MSG) __declspec(deprecated(MSG))
#else
#define META_DEPRECATED(MSG)
#endif
#endif
#else
#define META_DEPRECATED(MSG)
#endif

namespace meta
{
    inline namespace v1
    {
#ifdef __cpp_lib_integer_sequence
        using std::integer_sequence;
#else
        template <typename T, T...>
        struct integer_sequence;
#endif

        template <typename... Ts>
        struct list;

        template <typename T>
        struct id;

        template <template <typename...> class>
        struct quote;

        template <typename T, template <T...> class F>
        struct quote_i;

        template <typename... Fs>
        struct compose;

        template <template <typename...> class C, typename... Ts>
        struct defer;

        template <typename T, template <T...> class C, T... Is>
        struct defer_i;

        namespace extension
        {
            template <typename F, typename List>
            struct apply;
        }

    } // inline namespace v1
} // namespace meta

#endif
/// \file meta.hpp Tiny meta-programming library.
//
// Meta library
//
//  Copyright Eric Niebler 2014-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_HPP
#define META_HPP

//#include <cstddef>
//#include <initializer_list>
//#include <meta/meta_fwd.hpp>
//#include <type_traits>
//#include <utility>

#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdocumentation-deprecated-sync"
#endif

/// \defgroup meta Meta
///
/// A tiny metaprogramming library

/// \defgroup trait Trait
/// Trait invocation/composition.
/// \ingroup meta

/// \defgroup invocation Invocation
/// Trait invocation
/// \ingroup trait

/// \defgroup composition Composition
/// Trait composition
/// \ingroup trait

/// \defgroup logical Logical
/// Logical operations
/// \ingroup meta

/// \defgroup algorithm Algorithms
/// Algorithms.
/// \ingroup meta

/// \defgroup query Query/Search
/// Query and search algorithms
/// \ingroup algorithm

/// \defgroup transformation Transformation
/// Transformation algorithms
/// \ingroup algorithm

/// \defgroup runtime Runtime
/// Runtime algorithms
/// \ingroup algorithm

/// \defgroup datatype Datatype
/// Datatypes.
/// \ingroup meta

/// \defgroup list List
/// \ingroup datatype

/// \defgroup integral Integer sequence
/// Equivalent to C++14's `std::integer_sequence`
/// \ingroup datatype

/// \defgroup extension Extension
/// Extend meta with your own datatypes.
/// \ingroup datatype

/// \defgroup math Math
/// Integral constant arithmetic.
/// \ingroup meta

/// \defgroup lazy_trait lazy
/// \ingroup trait

/// \defgroup lazy_invocation lazy
/// \ingroup invocation

/// \defgroup lazy_composition lazy
/// \ingroup composition

/// \defgroup lazy_logical lazy
/// \ingroup logical

/// \defgroup lazy_query lazy
/// \ingroup query

/// \defgroup lazy_transformation lazy
/// \ingroup transformation

/// \defgroup lazy_list lazy
/// \ingroup list

/// \defgroup lazy_datatype lazy
/// \ingroup datatype

/// \defgroup lazy_math lazy
/// \ingroup math

/// Tiny metaprogramming library
namespace meta
{
    /// \cond
    inline namespace v1
    {
        /// \endcond

        namespace detail
        {
            /// Returns a \p T nullptr
            template <typename T>
            constexpr T *_nullptr_v()
            {
                return nullptr;
            }
        } // namespace detail

        /// An empty type.
        /// \ingroup datatype
        struct nil_
        {
        };

        /// Type alias for \p T::type.
        /// \ingroup invocation
        template <typename T>
        using _t = typename T::type;

#if defined(__cpp_variable_templates) || defined(META_DOXYGEN_INVOKED)
        /// Variable alias for \c T::type::value
        /// \note Requires C++14 or greater.
        /// \ingroup invocation
        template <typename T>
        constexpr typename _t<T>::value_type _v = _t<T>::value;
#endif

        /// Lazy versions of meta actions
        namespace lazy
        {
            /// \sa `meta::_t`
            /// \ingroup lazy_invocation
            template <typename T>
            using _t = defer<_t, T>;
        }

        /// An integral constant wrapper for \c std::size_t.
        /// \ingroup integral
        template <std::size_t N>
        using size_t = std::integral_constant<std::size_t, N>;

        /// An integral constant wrapper for \c bool.
        /// \ingroup integral
        template <bool B>
        using bool_ = std::integral_constant<bool, B>;

        /// An integral constant wrapper for \c int.
        /// \ingroup integral
        template <int I>
        using int_ = std::integral_constant<int, I>;

        /// An integral constant wrapper for \c char.
        /// \ingroup integral
        template <char Ch>
        using char_ = std::integral_constant<char, Ch>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Math operations
        /// An integral constant wrapper around the result of incrementing the wrapped
        /// integer \c
        /// T::type::value.
        template <typename T>
        using inc = std::integral_constant<decltype(T::type::value + 1), T::type::value + 1>;

        /// An integral constant wrapper around the result of decrementing the wrapped
        /// integer \c
        /// T::type::value.
        template <typename T>
        using dec = std::integral_constant<decltype(T::type::value - 1), T::type::value - 1>;

        /// An integral constant wrapper around the result of adding the two wrapped
        /// integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using plus = std::integral_constant<decltype(T::type::value + U::type::value),
                                            T::type::value + U::type::value>;

        /// An integral constant wrapper around the result of subtracting the two
        /// wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using minus = std::integral_constant<decltype(T::type::value - U::type::value),
                                             T::type::value - U::type::value>;

        /// An integral constant wrapper around the result of multiplying the two
        /// wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using multiplies = std::integral_constant<decltype(T::type::value * U::type::value),
                                                  T::type::value * U::type::value>;

        /// An integral constant wrapper around the result of dividing the two wrapped
        /// integers \c
        /// T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using divides = std::integral_constant<decltype(T::type::value / U::type::value),
                                               T::type::value / U::type::value>;

        /// An integral constant wrapper around the result of negating the wrapped
        /// integer
        /// \c T::type::value.
        /// \ingroup math
        template <typename T>
        using negate = std::integral_constant<decltype(-T::type::value), -T::type::value>;

        /// An integral constant wrapper around the remainder of dividing the two
        /// wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using modulus = std::integral_constant<decltype(T::type::value % U::type::value),
                                               T::type::value % U::type::value>;

        /// A Boolean integral constant wrapper around the result of comparing \c
        /// T::type::value and
        /// \c U::type::value for equality.
        /// \ingroup math
        template <typename T, typename U>
        using equal_to = bool_<T::type::value == U::type::value>;

        /// A Boolean integral constant wrapper around the result of comparing \c
        /// T::type::value and
        /// \c U::type::value for inequality.
        /// \ingroup math
        template <typename T, typename U>
        using not_equal_to = bool_<T::type::value != U::type::value>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// greater than
        /// \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using greater = bool_<(T::type::value > U::type::value)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// less than \c
        /// U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using less = bool_<(T::type::value < U::type::value)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// greater than
        /// or equal to \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using greater_equal = bool_<(T::type::value >= U::type::value)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// less than or
        /// equal to \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using less_equal = bool_<(T::type::value <= U::type::value)>;

        /// An integral constant wrapper around the result of bitwise-and'ing the two
        /// wrapped
        /// integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using bit_and = std::integral_constant<decltype(T::type::value & U::type::value),
                                               T::type::value & U::type::value>;

        /// An integral constant wrapper around the result of bitwise-or'ing the two
        /// wrapped
        /// integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using bit_or = std::integral_constant<decltype(T::type::value | U::type::value),
                                              T::type::value | U::type::value>;

        /// An integral constant wrapper around the result of bitwise-exclusive-or'ing
        /// the two
        /// wrapped integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using bit_xor = std::integral_constant<decltype(T::type::value ^ U::type::value),
                                               T::type::value ^ U::type::value>;

        /// An integral constant wrapper around the result of bitwise-complementing the
        /// wrapped
        /// integer \c T::type::value.
        /// \ingroup math
        template <typename T>
        using bit_not = std::integral_constant<decltype(~T::type::value), ~T::type::value>;

        namespace lazy
        {
            /// \sa 'meta::int'
            /// \ingroup lazy_math
            template <typename T>
            using inc = defer<inc, T>;

            /// \sa 'meta::dec'
            /// \ingroup lazy_math
            template <typename T>
            using dec = defer<dec, T>;

            /// \sa 'meta::plus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using plus = defer<plus, T, U>;

            /// \sa 'meta::minus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using minus = defer<minus, T, U>;

            /// \sa 'meta::multiplies'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using multiplies = defer<multiplies, T, U>;

            /// \sa 'meta::divides'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using divides = defer<divides, T, U>;

            /// \sa 'meta::negate'
            /// \ingroup lazy_math
            template <typename T>
            using negate = defer<negate, T>;

            /// \sa 'meta::modulus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using modulus = defer<modulus, T, U>;

            /// \sa 'meta::equal_to'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using equal_to = defer<equal_to, T, U>;

            /// \sa 'meta::not_equal_t'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using not_equal_to = defer<not_equal_to, T, U>;

            /// \sa 'meta::greater'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using greater = defer<greater, T, U>;

            /// \sa 'meta::less'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using less = defer<less, T, U>;

            /// \sa 'meta::greater_equal'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using greater_equal = defer<greater_equal, T, U>;

            /// \sa 'meta::less_equal'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using less_equal = defer<less_equal, T, U>;

            /// \sa 'meta::bit_and'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_and = defer<bit_and, T, U>;

            /// \sa 'meta::bit_or'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_or = defer<bit_or, T, U>;

            /// \sa 'meta::bit_xor'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_xor = defer<bit_xor, T, U>;

            /// \sa 'meta::bit_not'
            /// \ingroup lazy_math
            template <typename T>
            using bit_not = defer<bit_not, T>;
        }

        /// \cond
        namespace detail
        {
            enum class indices_strategy_
            {
                done,
                repeat,
                recurse
            };

            constexpr indices_strategy_ strategy_(std::size_t cur, std::size_t end)
            {
                return cur >= end ? indices_strategy_::done
                                  : cur * 2 <= end ? indices_strategy_::repeat
                                                   : indices_strategy_::recurse;
            }

            template <typename T>
            constexpr std::size_t range_distance_(T begin, T end)
            {
                return begin <= end ? static_cast<std::size_t>(end - begin)
                                    : throw "The start of the integer_sequence must not be "
                                            "greater than the end";
            }

            template <std::size_t End, typename State, indices_strategy_ Status>
            struct make_indices_
            {
                using type = State;
            };

            template <typename T, T, typename>
            struct coerce_indices_
            {
            };
        }
/// \endcond

///////////////////////////////////////////////////////////////////////////////////////////////
// integer_sequence
#ifndef __cpp_lib_integer_sequence
        /// A container for a sequence of compile-time integer constants.
        /// \ingroup integral
        template <typename T, T... Is>
        struct integer_sequence
        {
            using value_type = T;
            /// \return `sizeof...(Is)`
            static constexpr std::size_t size() noexcept { return sizeof...(Is); }
        };
#endif

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // index_sequence
        /// A container for a sequence of compile-time integer constants of type
        /// \c std::size_t
        /// \ingroup integral
        template <std::size_t... Is>
        using index_sequence = integer_sequence<std::size_t, Is...>;

#if !defined(META_DOXYGEN_INVOKED) &&                                        \
    ((defined(__clang__) && __clang_major__ >= 3 && __clang_minor__ >= 8) || \
     (defined(_MSC_VER) && _MSC_FULL_VER >= 190023918))
        // Implement make_integer_sequence and make_index_sequence with the
        // __make_integer_seq builtin on compilers that provide it. (Redirect
        // through decltype to workaround suspected clang bug.)
        /// \cond
        namespace detail
        {
            template <class T, T N>
            __make_integer_seq<integer_sequence, T, N> make_integer_sequence_();
        }
        /// \endcond

        template <typename T, T N>
        using make_integer_sequence = decltype(detail::make_integer_sequence_<T, N>());

        template <std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;
#else
        /// Generate \c index_sequence containing integer constants [0,1,2,...,N-1].
        /// \par Complexity
        /// \f$ O(log(N)) \f$.
        /// \ingroup integral
        template <std::size_t N>
        using make_index_sequence =
            _t<detail::make_indices_<N, index_sequence<0>, detail::strategy_(1, N)>>;

        /// Generate \c integer_sequence containing integer constants [0,1,2,...,N-1].
        /// \par Complexity
        /// \f$ O(log(N)) \f$.
        /// \ingroup integral
        template <typename T, T N>
        using make_integer_sequence =
            _t<detail::coerce_indices_<T, 0, make_index_sequence<static_cast<std::size_t>(N)>>>;
#endif

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // integer_range
        /// Makes the integer sequence <tt>[From, To)</tt>.
        /// \par Complexity
        /// \f$ O(log(To - From)) \f$.
        /// \ingroup integral
        template <class T, T From, T To>
        using integer_range =
            _t<detail::coerce_indices_<T, From,
                                       make_index_sequence<detail::range_distance_(From, To)>>>;

        /// \cond
        namespace detail
        {
            template <typename, typename>
            struct concat_indices_
            {
            };

            template <std::size_t... Is, std::size_t... Js>
            struct concat_indices_<index_sequence<Is...>, index_sequence<Js...>>
            {
                using type = index_sequence<Is..., (Js + sizeof...(Is))...>;
            };

            template <>
            struct make_indices_<0u, index_sequence<0>, indices_strategy_::done>
            {
                using type = index_sequence<>;
            };

            template <std::size_t End, std::size_t... Values>
            struct make_indices_<End, index_sequence<Values...>, indices_strategy_::repeat>
                : make_indices_<End, index_sequence<Values..., (Values + sizeof...(Values))...>,
                                detail::strategy_(sizeof...(Values)*2, End)>
            {
            };

            template <std::size_t End, std::size_t... Values>
            struct make_indices_<End, index_sequence<Values...>, indices_strategy_::recurse>
                : concat_indices_<index_sequence<Values...>,
                                  make_index_sequence<End - sizeof...(Values)>>
            {
            };

            template <typename T, T Offset, std::size_t... Values>
            struct coerce_indices_<T, Offset, index_sequence<Values...>>
            {
                using type =
                    integer_sequence<T, static_cast<T>(static_cast<T>(Values) + Offset)...>;
            };
        } // namespace detail
        /// \endcond

        /// Evaluate the Callable \p F with the arguments \p Args.
        /// \ingroup invocation
        template <typename F, typename... Args>
        using invoke = typename F::template invoke<Args...>;

        /// Lazy versions of meta actions
        namespace lazy
        {
            /// \sa `meta::invoke`
            /// \ingroup lazy_invocation
            template <typename F, typename... Args>
            using invoke = defer<invoke, F, Args...>;
        }

        /// A trait that always returns its argument \p T. Also, a Callable that always
        /// returns
        /// \p T.
        /// \ingroup trait
        /// \ingroup invocation
        template <typename T>
        struct id
        {
            /// \cond
            // Redirect through decltype for compilers that have not
            // yet implemented CWG 1558:
            // <http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1558>
            static id impl(void *);
            /// \endcond

            using type = T;

            template <typename... Ts>
            using invoke = _t<decltype(id::impl(static_cast<list<Ts...> *>(nullptr)))>;
        };

        /// An alias for type \p T. Useful in non-deduced contexts.
        /// \ingroup trait
        template <typename T>
        using id_t = _t<id<T>>;

        namespace lazy
        {
            /// \sa `meta::id`
            /// \ingroup lazy_trait
            /// \ingroup lazy_invocation
            template <typename T>
            using id = defer<id, T>;
        }

        /// An alias for `void`.
        /// \ingroup trait
        template <typename... Ts>
        using void_ = invoke<id<void>, Ts...>;

        /// \cond
        namespace detail
        {
            template <typename, typename = void>
            struct is_trait_
            {
                using type = std::false_type;
            };

            template <typename T>
            struct is_trait_<T, void_<typename T::type>>
            {
                using type = std::true_type;
            };

            template <typename, typename = void>
            struct is_callable_
            {
                using type = std::false_type;
            };

            template <typename T>
            struct is_callable_<T, void_<quote<T::template invoke>>>
            {
                using type = std::true_type;
            };

            struct defer_if_
            {
                template <template <typename...> class C, typename... Ts>
                struct result
                {
                    using type = C<Ts...>;
                };
                template <template <typename...> class C, typename... Ts,
                    typename = C<Ts...>>
                result<C, Ts...> try_();
                template <template <typename...> class C, typename... Ts>
                nil_ try_() const;
            };

            struct defer_i_if_
            {
                template <typename T, template <T...> class C, T... Is>
                struct result
                {
                    using type = C<Is...>;
                };
                template <typename T, template <T...> class C, T... Is,
                    typename = C<Is...>>
                result<T, C, Is...> try_();
                template <typename T, template <T...> class C, T... Is>
                nil_ try_() const;
            };

            template <template <typename...> class C, typename... Ts>
            using defer_ = decltype(defer_if_{}.try_<C, Ts...>());

            template <typename T, template <T...> class C, T... Is>
            using defer_i_ = decltype(defer_i_if_{}.try_<T, C, Is...>());

            template <typename T>
            using _t_t = _t<_t<T>>;
        } // namespace detail
        /// \endcond

        /// An alias for `std::true_type` if `T::type` exists and names a type;
        /// otherwise, it's an
        /// alias for `std::false_type`.
        /// \ingroup trait
        template <typename T>
        using is_trait = _t<detail::is_trait_<T>>;

        /// An alias for `std::true_type` if `T::invoke` exists and names a class
        /// template or
        /// alias template; otherwise, it's an alias for `std::false_type`.
        /// \ingroup trait
        template <typename T>
        using is_callable = _t<detail::is_callable_<T>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer
        /// A wrapper that defers the instantiation of a template \p C with type
        /// parameters \p Ts in
        /// a \c lambda or \c let expression.
        ///
        /// In the code below, the lambda would ideally be written as
        /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since `push_back`
        /// expects its first
        /// argument to be a list, not a placeholder. Instead, we express it using \c
        /// defer as
        /// follows:
        ///
        /// \code
        /// template<typename List>
        /// using reverse = reverse_fold<List, list<>, lambda<_a, _b, defer<push_back,
        /// _a, _b>>>;
        /// \endcode
        ///
        /// \ingroup invocation
        template <template <typename...> class C, typename... Ts>
        struct defer : detail::defer_<C, Ts...>
        {
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_i
        /// A wrapper that defers the instantiation of a template \p C with integral
        /// constant
        /// parameters \p Is in a \c lambda or \c let expression.
        /// \sa `defer`
        /// \ingroup invocation
        template <typename T, template <T...> class C, T... Is>
        struct defer_i : detail::defer_i_<T, C, Is...>
        {
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_trait
        /// A wrapper that defers the instantiation of a trait \p C with type parameters
        /// \p Ts in a
        /// \c lambda or \c let expression.
        /// \sa `defer`
        /// \ingroup invocation
        template <template <typename...> class C, typename... Ts>
        using defer_trait = defer<detail::_t_t, detail::defer_<C, Ts...>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_trait_i
        /// A wrapper that defers the instantiation of a trait \p C with integral
        /// constant
        /// parameters \p Is in a \c lambda or \c let expression.
        /// \sa `defer_i`
        /// \ingroup invocation
        template <typename T, template <T...> class C, T... Is>
        using defer_trait_i =
            defer<detail::_t_t, detail::defer_i_<T, C, Is...>>;

        /// An alias that computes the size of the type \p T.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup trait
        template <class T>
        using sizeof_ = meta::size_t<sizeof(T)>;

        /// An alias that computes the alignment required for any instance of the type
        /// \p T.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup trait
        template <class T>
        using alignof_ = meta::size_t<alignof(T)>;

        namespace lazy
        {
            /// \sa `meta::sizeof_`
            /// \ingroup lazy_trait
            template <typename T>
            using sizeof_ = defer<sizeof_, T>;

            /// \sa `meta::alignof_`
            /// \ingroup lazy_trait
            template <typename T>
            using alignof_ = defer<alignof_, T>;
        }

        /// is
        /// \cond
        namespace detail
        {
            template <typename, template <typename...> class>
            struct is_ : std::false_type
            {
            };

            template <typename... Ts, template <typename...> class C>
            struct is_<C<Ts...>, C> : std::true_type
            {
            };
        }
        /// \endcond

        /// Test whether a type \c T is an instantiation of class
        /// template \t C.
        /// \ingroup trait
        template <typename T, template <typename...> class C>
        using is = _t<detail::is_<T, C>>;

        /// Compose the Callables \p Fs in the parameter pack \p Ts.
        /// \ingroup composition
        template <typename... Fs>
        struct compose
        {
        };

        template <typename F0>
        struct compose<F0>
        {
            template <typename... Ts>
            using invoke = invoke<F0, Ts...>;
        };

        template <typename F0, typename... Fs>
        struct compose<F0, Fs...>
        {
            template <typename... Ts>
            using invoke = invoke<F0, invoke<compose<Fs...>, Ts...>>;
        };

        namespace lazy
        {
            /// \sa 'meta::compose'
            /// \ingroup lazy_composition
            template <typename... Fns>
            using compose = defer<compose, Fns...>;
        }

        /// Turn a class template or alias template \p C into a Callable.
        /// \ingroup composition
        template <template <typename...> class C>
        struct quote
        {
            // Indirection through defer here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename... Ts>
            using invoke = _t<detail::defer_<C, Ts...>>;
        };

        /// Turn a class template or alias template \p C taking literals of type \p T
        /// into a Callable.
        /// \ingroup composition
        template <typename T, template <T...> class C>
        struct quote_i
        {
            // Indirection through defer_i here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename... Ts>
            using invoke = _t<detail::defer_i_<T, C, Ts::type::value...>>;
        };

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 8 && !defined(META_DOXYGEN_INVOKED)
        template <template <typename...> class C>
        struct quote_trait
        {
            template <typename... Ts>
            using invoke = _t<invoke<quote<C>, Ts...>>;
        };

        template <typename T, template <T...> class C>
        struct quote_trait_i
        {
            template <typename... Ts>
            using invoke = _t<invoke<quote_i<T, C>, Ts...>>;
        };
#else
        // clang-format off
        /// Turn a trait template \p C into a Callable.
        /// \code
        /// static_assert(std::is_same<invoke<quote_trait<std::add_const>, int>, int const>::value, "");
        /// \endcode
        /// \ingroup composition
        template <template <typename...> class C>
        using quote_trait = compose<quote<_t>, quote<C>>;

        /// Turn a trait \p C taking literals of type \p T into a Callable.
        /// \sa 'meta::quote_trait'
        /// \ingroup composition
        template <typename T, template <T...> class C>
        using quote_trait_i = compose<quote<_t>, quote_i<T, C>>;
// clang-format on
#endif

        /// A Callable that partially applies the Callable
        /// \p F by binding the arguments \p Ts to the \e front of \p F.
        /// \ingroup composition
        template <typename F, typename... Ts>
        struct bind_front
        {
            template <typename... Us>
            using invoke = invoke<F, Ts..., Us...>;
        };

        /// A Callable that partially applies the Callable \p F by binding the
        /// arguments \p Us to the \e back of \p F.
        /// \ingroup composition
        template <typename F, typename... Us>
        struct bind_back
        {
            template <typename... Ts>
            using invoke = invoke<F, Ts..., Us...>;
        };

        namespace lazy
        {
            /// \sa 'meta::bind_front'
            /// \ingroup lazy_composition
            template <typename Fn, typename... Ts>
            using bind_front = defer<bind_front, Fn, Ts...>;

            /// \sa 'meta::bind_back'
            /// \ingroup lazy_composition
            template <typename Fn, typename... Ts>
            using bind_back = defer<bind_back, Fn, Ts...>;
        }

        /// Extend meta with your own datatypes.
        namespace extension
        {
            /// A trait that unpacks the types in the type list \p List into the Callable
            /// \p F.
            /// \ingroup extension
            template <typename F, typename List>
            struct apply
            {
            };

            template <typename F, typename Ret, typename... Args>
            struct apply<F, Ret(Args...)> : lazy::invoke<F, Ret, Args...>
            {
            };

            template <typename F, template <typename...> class T, typename... Ts>
            struct apply<F, T<Ts...>> : lazy::invoke<F, Ts...>
            {
            };

            template <typename F, typename T, T... Is>
            struct apply<F, integer_sequence<T, Is...>>
                : lazy::invoke<F, std::integral_constant<T, Is>...>
            {
            };
        }

        /// Applies the Callable \p C using the types in the type list \p List as
        /// arguments.
        /// \ingroup invocation
        template <typename C, typename List>
        using apply = _t<extension::apply<C, List>>;

        namespace lazy
        {
            template <typename F, typename List>
            using apply = defer<apply, F, List>;
        }

        /// A Callable that takes a bunch of arguments, bundles them into a type list,
        /// and
        /// then calls the Callable \p F with the type list \p Q.
        /// \ingroup composition
        template <typename F, typename Q = quote<list>>
        using curry = compose<F, Q>;

        /// A Callable that takes a type list, unpacks the types, and then calls the
        /// Callable \p F with the types.
        /// \ingroup composition
        template <typename F>
        using uncurry = bind_front<quote<apply>, F>;

        namespace lazy
        {
            /// \sa 'meta::curry'
            /// \ingroup lazy_composition
            template <typename F, typename Q = quote<list>>
            using curry = defer<curry, F, Q>;

            /// \sa 'meta::uncurry'
            /// \ingroup lazy_composition
            template <typename F>
            using uncurry = defer<uncurry, F>;
        }

        /// A Callable that reverses the order of the first two arguments.
        /// \ingroup composition
        template <typename F>
        struct flip
        {
        private:
            template <typename... Ts>
            struct impl
            {
            };
            template <typename A, typename B, typename... Ts>
            struct impl<A, B, Ts...> : lazy::invoke<F, B, A, Ts...>
            {
            };

        public:
            template <typename... Ts>
            using invoke = _t<impl<Ts...>>;
        };

        namespace lazy
        {
            /// \sa 'meta::flip'
            /// \ingroup lazy_composition
            template <typename F>
            using flip = defer<flip, F>;
        }

        /// \cond
        namespace detail
        {
            template <typename...>
            struct on_
            {
            };
            template <typename F, typename... Gs>
            struct on_<F, Gs...>
            {
                template <typename... Ts>
                using invoke = invoke<F, invoke<compose<Gs...>, Ts>...>;
            };
        }
        /// \endcond

        /// Use as `on<F, Gs...>`. Creates an Callable that applies Callable \c F to the
        /// result of applying Callable `compose<Gs...>` to all the arguments.
        /// \ingroup composition
        template <typename... Fs>
        using on = detail::on_<Fs...>;

        namespace lazy
        {
            /// \sa 'meta::on'
            /// \ingroup lazy_composition
            template <typename F, typename G>
            using on = defer<on, F, G>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // if_
        /// \cond
        namespace detail
        {
        #ifdef __clang__
            // Clang is faster with this implementation
            template <typename, typename = bool>
            struct _if_
            {
            };

            template <typename If>
            struct _if_<list<If>, decltype(bool(If::type::value))> : std::enable_if<If::type::value>
            {
            };

            template <typename If, typename Then>
            struct _if_<list<If, Then>, decltype(bool(If::type::value))>
                : std::enable_if<If::type::value, Then>
            {
            };

            template <typename If, typename Then, typename Else>
            struct _if_<list<If, Then, Else>, decltype(bool(If::type::value))>
                : std::conditional<If::type::value, Then, Else>
            {
            };
        #else
            // GCC seems to prefer this implementation
            template <typename, typename = std::true_type>
            struct _if_
            {
            };

            template <typename If>
            struct _if_<list<If>, bool_<If::type::value>>
            {
                using type = void;
            };

            template <typename If, typename Then>
            struct _if_<list<If, Then>, bool_<If::type::value>>
            {
                using type = Then;
            };

            template <typename If, typename Then, typename Else>
            struct _if_<list<If, Then, Else>, bool_<If::type::value>>
            {
                using type = Then;
            };

            template <typename If, typename Then, typename Else>
            struct _if_<list<If, Then, Else>, bool_<!If::type::value>>
            {
                using type = Else;
            };
        #endif
        } // namespace detail
        /// \endcond

        /// Select one type or another depending on a compile-time Boolean.
        /// \ingroup logical
        template <typename... Args>
        using if_ = _t<detail::_if_<list<Args...>>>;

        /// Select one type or another depending on a compile-time Boolean.
        /// \ingroup logical
        template <bool If, typename... Args>
        using if_c = _t<detail::_if_<list<bool_<If>, Args...>>>;

        namespace lazy
        {
            /// \sa 'meta::if_'
            /// \ingroup lazy_logical
            template <typename... Args>
            using if_ = defer<if_, Args...>;

            /// \sa 'meta::if_c'
            /// \ingroup lazy_logical
            template <bool If, typename... Args>
            using if_c = if_<bool_<If>, Args...>;
        }

        /// \cond
        namespace detail
        {
            template <typename... Bools>
            struct _and_;

            template <>
            struct _and_<> : std::true_type
            {
            };

            template <typename Bool, typename... Bools>
            struct _and_<Bool, Bools...>
                : if_c<!Bool::type::value, std::false_type, _and_<Bools...>>
            {
            };

            template <typename... Bools>
            struct _or_;

            template <>
            struct _or_<> : std::false_type
            {
            };

            template <typename Bool, typename... Bools>
            struct _or_<Bool, Bools...> : if_c<Bool::type::value, std::true_type, _or_<Bools...>>
            {
            };
        } // namespace detail
        /// \endcond

        /// Logically negate the Boolean parameter
        /// \ingroup logical
        template <bool Bool>
        using not_c = bool_<!Bool>;

        /// Logically negate the integral constant-wrapped Boolean parameter.
        /// \ingroup logical
        template <typename Bool>
        using not_ = not_c<Bool::type::value>;

/// Logically and together all the Boolean parameters
/// \ingroup logical
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 5 && __GNUC_MINOR__ == 1
        // Alternative formulation of and_c to workaround
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66405
        template <bool... Bools>
        using and_c = std::is_same<integer_sequence<bool, true, Bools...>,
                                   integer_sequence<bool, Bools..., true>>;
#else
        template <bool... Bools>
        using and_c = std::is_same<integer_sequence<bool, Bools...>,
                                   integer_sequence<bool, (Bools || true)...>>;
#endif

        /// Logically and together all the integral constant-wrapped Boolean parameters,
        /// \e without
        /// doing short-circuiting.
        /// \ingroup logical
        template <typename... Bools>
        using strict_and = and_c<Bools::type::value...>;

        /// Logically and together all the integral constant-wrapped Boolean parameters,
        /// \e with
        /// short-circuiting.
        /// \ingroup logical
        template <typename... Bools>
        using and_ = _t<detail::_and_<Bools...>>;

        /// Logically or together all the Boolean parameters
        /// \ingroup logical
        template <bool... Bools>
        using or_c = not_<std::is_same<integer_sequence<bool, Bools...>,
                                       integer_sequence<bool, (Bools && false)...>>>;

        /// Logically or together all the integral constant-wrapped Boolean parameters,
        /// \e without
        /// doing short-circuiting.
        /// \ingroup logical
        template <typename... Bools>
        using strict_or = or_c<Bools::type::value...>;

        /// Logically or together all the integral constant-wrapped Boolean parameters,
        /// \e with
        /// short-circuiting.
        /// \ingroup logical
        template <typename... Bools>
        using or_ = _t<detail::_or_<Bools...>>;

        namespace lazy
        {
            /// \sa 'meta::and_'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using and_ = defer<and_, Bools...>;

            /// \sa 'meta::or_'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using or_ = defer<or_, Bools...>;

            /// \sa 'meta::not_'
            /// \ingroup lazy_logical
            template <typename Bool>
            using not_ = defer<not_, Bool>;

            /// \sa 'meta::strict_and'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using strict_and = defer<strict_and, Bools...>;

            /// \sa 'meta::strict_or'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using strict_or = defer<strict_or, Bools...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // fold
        /// \cond
        namespace detail
        {
            template <typename Fun, typename T0>
            struct compose1_
            {
                template <typename X>
                using invoke = invoke<Fun, _t<X>, T0>;
            };

            template <typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4,
                      typename T5, typename T6, typename T7, typename T8, typename T9>
            struct compose10_
            {
                template <typename X, typename Y>
                using F = invoke<Fun, X, Y>;

                template <typename S>
                using invoke =
                    F<F<F<F<F<F<F<F<F<F<_t<S>, T0>, T1>, T2>, T3>, T4>, T5>, T6>, T7>, T8>, T9>;
            };

            template <typename, typename, typename>
            struct fold_
            {
            };

            template <typename State, typename Fun>
            struct fold_<list<>, State, Fun> : State
            {
            };

            template <typename Head, typename... List, typename State, typename Fun>
            struct fold_<list<Head, List...>, State, Fun>
                : fold_<list<List...>, lazy::invoke<compose1_<Fun, Head>, State>, Fun>
            {
            };

            template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
                      typename T6, typename T7, typename T8, typename T9, typename... List,
                      typename State, typename Fun>
            struct fold_<list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, List...>, State, Fun>
                : fold_<
                      list<List...>,
                      lazy::invoke<compose10_<Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>, State>,
                      Fun>
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by doing a left fold of the list \p List using
        /// binary Callable \p Fun and initial state \p State. That is, the \c State_N for the list
        /// element \c A_N is computed by `Fun(State_N-1, A_N) -> State_N`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename State, typename Fun>
        using fold = _t<detail::fold_<List, id<State>, Fun>>;

        /// An alias for `meta::fold`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename State, typename Fun>
        using accumulate = fold<List, State, Fun>;

        namespace lazy
        {
            /// \sa 'meta::foldl'
            /// \ingroup lazy_transformation
            template <typename List, typename State, typename Fun>
            using fold = defer<fold, List, State, Fun>;

            /// \sa 'meta::accumulate'
            /// \ingroup lazy_transformation
            template <typename List, typename State, typename Fun>
            using accumulate = defer<accumulate, List, State, Fun>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse_fold
        /// \cond
        namespace detail
        {
            template <typename List, typename State, typename Fun>
            struct reverse_fold_
            {
            };

            template <typename State, typename Fun>
            struct reverse_fold_<list<>, State, Fun>
            {
                using type = State;
            };

            template <typename Head, typename... List, typename State, typename Fun>
            struct reverse_fold_<list<Head, List...>, State, Fun>
                : lazy::invoke<compose1_<Fun, Head>, reverse_fold_<list<List...>, State, Fun>>
            {
            };

            template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
                      typename T6, typename T7, typename T8, typename T9, typename... List,
                      typename State, typename Fun>
            struct reverse_fold_<list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, List...>, State, Fun>
                : lazy::invoke<compose10_<Fun, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0>,
                               reverse_fold_<list<List...>, State, Fun>>
            {
            };
        }
        /// \endcond

        /// Return a new \c meta::list constructed by doing a right fold of the list \p List using
        /// binary Callable \p Fun and initial state \p State. That is, the \c State_N
        /// for the list element \c A_N is computed by `Fun(A_N, State_N+1) -> State_N`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename State, typename Fun>
        using reverse_fold = _t<detail::reverse_fold_<List, State, Fun>>;

        namespace lazy
        {
            /// \sa 'meta::foldr'
            /// \ingroup lazy_transformation
            template <typename List, typename State, typename Fun>
            using reverse_fold = defer<reverse_fold, List, State, Fun>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // npos
        /// A special value used to indicate no matches. It equals the maximum value
        /// representable
        /// by std::size_t.
        /// \ingroup list
        using npos = meta::size_t<std::size_t(-1)>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // list
        /// A list of types.
        /// \ingroup list
        template <typename... Ts>
        struct list
        {
            using type = list;
            /// \return `sizeof...(Ts)`
            static constexpr std::size_t size() noexcept { return sizeof...(Ts); }
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // size
        /// An integral constant wrapper that is the size of the \c meta::list
        /// \p List.
        /// \ingroup list
        template <typename List>
        using size = meta::size_t<List::size()>;

        namespace lazy
        {
            /// \sa 'meta::size'
            /// \ingroup lazy_list
            template <typename List>
            using size = defer<size, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // concat
        /// \cond
        namespace detail
        {
            template <typename... Lists>
            struct concat_
            {
            };

            template <>
            struct concat_<>
            {
                using type = list<>;
            };

            template <typename... List1>
            struct concat_<list<List1...>>
            {
                using type = list<List1...>;
            };

            template <typename... List1, typename... List2, typename... Rest>
            struct concat_<list<List1...>, list<List2...>, Rest...>
                : concat_<list<List1..., List2...>, Rest...>
            {
            };

            template <typename... List1, typename... List2, typename... List3, typename... List4,
                      typename... List5, typename... List6, typename... List7, typename... List8,
                      typename... List9, typename... List10, typename... Rest>
            struct concat_<list<List1...>, list<List2...>, list<List3...>, list<List4...>,
                           list<List5...>, list<List6...>, list<List7...>, list<List8...>,
                           list<List9...>, list<List10...>, Rest...>
                : concat_<list<List1..., List2..., List3..., List4..., List5..., List6..., List7...,
                               List8..., List9..., List10...>,
                          Rest...>
            {
            };
        } // namespace detail
        /// \endcond

        /// Concatenates several lists into a single list.
        /// \pre The parameters must all be instantiations of \c meta::list.
        /// \par Complexity
        /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of lists.
        /// \ingroup transformation
        template <typename... Lists>
        using concat = _t<detail::concat_<Lists...>>;

        namespace lazy
        {
            /// \sa 'meta::concat'
            /// \ingroup lazy_transformation
            template <typename... Lists>
            using concat = defer<concat, Lists...>;
        }

        /// Joins a list of lists into a single list.
        /// \pre The parameter must be an instantiation of \c meta::list\<T...\>
        /// where each \c T is itself an instantiation of \c meta::list.
        /// \par Complexity
        /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
        /// lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using join = apply<quote<concat>, ListOfLists>;

        namespace lazy
        {
            /// \sa 'meta::join'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using join = defer<join, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // transform
        /// \cond
        namespace detail
        {
            template <typename, typename = void>
            struct transform_
            {
            };

            template <typename... Ts, typename Fun>
            struct transform_<list<list<Ts...>, Fun>, void_<invoke<Fun, Ts>...>>
            {
                using type = list<invoke<Fun, Ts>...>;
            };

            template <typename... Ts0, typename... Ts1, typename Fun>
            struct transform_<list<list<Ts0...>, list<Ts1...>, Fun>,
                              void_<invoke<Fun, Ts0, Ts1>...>>
            {
                using type = list<invoke<Fun, Ts0, Ts1>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by transforming all the elements in
        /// \p List with
        /// the unary Callable \p Fun. \c transform can also be called with two lists of
        /// the same length and a binary Callable, in which case it returns a new list
        /// constructed with the results of calling \c Fun with each element in the
        /// lists,
        /// pairwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename... Args>
        using transform = _t<detail::transform_<list<Args...>>>;

        namespace lazy
        {
            /// \sa 'meta::transform'
            /// \ingroup lazy_transformation
            template <typename... Args>
            using transform = defer<transform, Args...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // repeat_n
        /// \cond
        namespace detail
        {
            template <typename T, std::size_t>
            using first_ = T;

            template <typename T, typename Ints>
            struct repeat_n_c_
            {
            };

            template <typename T, std::size_t... Is>
            struct repeat_n_c_<T, index_sequence<Is...>>
            {
                using type = list<first_<T, Is>...>;
            };
        }
        /// \endcond

        /// Generate `list<T,T,T...T>` of size \p N arguments.
        /// \par Complexity
        /// \f$ O(log N) \f$.
        /// \ingroup list
        template <std::size_t N, typename T = void>
        using repeat_n_c = _t<detail::repeat_n_c_<T, make_index_sequence<N>>>;

        /// Generate `list<T,T,T...T>` of size \p N arguments.
        /// \par Complexity
        /// \f$ O(log N) \f$.
        /// \ingroup list
        template <typename N, typename T = void>
        using repeat_n = repeat_n_c<N::type::value, T>;

        namespace lazy
        {
            /// \sa 'meta::repeat_n'
            /// \ingroup lazy_list
            template <typename N, typename T = void>
            using repeat_n = defer<repeat_n, N, T>;

            /// \sa 'meta::repeat_n_c'
            /// \ingroup lazy_list
            template <std::size_t N, typename T = void>
            using repeat_n_c = defer<repeat_n, meta::size_t<N>, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // at
        /// \cond
        namespace detail
        {
            template <typename VoidPtrs>
            struct at_impl_;

            template <typename... VoidPtrs>
            struct at_impl_<list<VoidPtrs...>>
            {
                static nil_ eval(...);

                template <typename T, typename... Us>
                static T eval(VoidPtrs..., T *, Us *...);
            };

            template <typename List, typename N>
            struct at_
            {
            };

            template <typename... Ts, typename N>
            struct at_<list<Ts...>, N>
                : decltype(at_impl_<repeat_n<N, void *>>::eval(static_cast<id<Ts> *>(nullptr)...))
            {
            };
        } // namespace detail
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // at
        /// Return the \p N th element in the \c meta::list \p List.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <typename List, typename N>
        using at = _t<detail::at_<List, N>>;

        /// Return the \p N th element in the \c meta::list \p List.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <typename List, std::size_t N>
        using at_c = at<List, meta::size_t<N>>;

        namespace lazy
        {
            /// \sa 'meta::at'
            /// \ingroup lazy_list
            template <typename List, typename N>
            using at = defer<at, List, N>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // drop
        /// \cond
        namespace detail
        {
            ///////////////////////////////////////////////////////////////////////////////////////
            /// drop_impl_
            template <typename VoidPtrs>
            struct drop_impl_
            {
                static nil_ eval(...);
            };

            template <typename... VoidPtrs>
            struct drop_impl_<list<VoidPtrs...>>
            {
                static nil_ eval(...);

                template <typename... Ts>
                static id<list<Ts...>> eval(VoidPtrs..., id<Ts> *...);
            };

            template <>
            struct drop_impl_<list<>>
            {
                template <typename... Ts>
                static id<list<Ts...>> eval(id<Ts> *...);
            };

            template <typename List, typename N>
            struct drop_
            {
            };

            template <typename... Ts, typename N>
            struct drop_<list<Ts...>, N>
                : decltype(drop_impl_<repeat_n<N, void *>>::eval(detail::_nullptr_v<id<Ts>>()...))
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by removing the first \p N elements from \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List, typename N>
        using drop = _t<detail::drop_<List, N>>;

        /// Return a new \c meta::list by removing the first \p N elements from \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List, std::size_t N>
        using drop_c = _t<detail::drop_<List, meta::size_t<N>>>;

        namespace lazy
        {
            /// \sa 'meta::drop'
            /// \ingroup lazy_transformation
            template <typename List, typename N>
            using drop = defer<drop, List, N>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // front
        /// \cond
        namespace detail
        {
            template <typename List>
            struct front_
            {
            };

            template <typename Head, typename... List>
            struct front_<list<Head, List...>>
            {
                using type = Head;
            };
        } // namespace detail
        /// \endcond

        /// Return the first element in \c meta::list \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup list
        template <typename List>
        using front = _t<detail::front_<List>>;

        namespace lazy
        {
            /// \sa 'meta::front'
            /// \ingroup lazy_list
            template <typename List>
            using front = defer<front, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // back
        /// \cond
        namespace detail
        {
            template <typename List>
            struct back_
            {
            };

            template <typename Head, typename... List>
            struct back_<list<Head, List...>>
            {
                using type = at_c<list<Head, List...>, sizeof...(List)>;
            };
        } // namespace detail
        /// \endcond

        /// Return the last element in \c meta::list \p List.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <typename List>
        using back = _t<detail::back_<List>>;

        namespace lazy
        {
            /// \sa 'meta::back'
            /// \ingroup lazy_list
            template <typename List>
            using back = defer<back, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // push_front
        /// \cond
        namespace detail
        {
            template <typename List, typename T>
            struct push_front_
            {
            };

            template <typename... List, typename T>
            struct push_front_<list<List...>, T>
            {
                using type = list<T, List...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by adding the element \c T to the front of \p
        /// List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List, typename T>
        using push_front = _t<detail::push_front_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::push_front'
            /// \ingroup lazy_transformation
            template <typename List, typename T>
            using push_front = defer<push_front, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // pop_front
        /// \cond
        namespace detail
        {
            template <typename List>
            struct pop_front_
            {
            };

            template <typename Head, typename... List>
            struct pop_front_<list<Head, List...>>
            {
                using type = list<List...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by removing the first element from the front of
        /// \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List>
        using pop_front = _t<detail::pop_front_<List>>;

        namespace lazy
        {
            /// \sa 'meta::pop_front'
            /// \ingroup lazy_transformation
            template <typename List>
            using pop_front = defer<pop_front, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // push_back
        /// \cond
        namespace detail
        {
            template <typename List, typename T>
            struct push_back_
            {
            };

            template <typename... List, typename T>
            struct push_back_<list<List...>, T>
            {
                using type = list<List..., T>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by adding the element \c T to the back of \p
        /// List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \note \c pop_back not provided because it cannot be made to meet the
        /// complexity guarantees one would expect.
        /// \ingroup transformation
        template <typename List, typename T>
        using push_back = _t<detail::push_back_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::push_back'
            /// \ingroup lazy_transformation
            template <typename List, typename T>
            using push_back = defer<push_back, List, T>;
        }

        /// \cond
        namespace detail
        {
            template <typename T, typename U>
            using min_ = if_<less<U, T>, U, T>;

            template <typename T, typename U>
            using max_ = if_<less<U, T>, T, U>;
        }
        /// \endcond

        /// An integral constant wrapper around the minimum of `Ts::type::value...`
        /// \ingroup math
        template <typename... Ts>
        using min = fold<pop_front<list<Ts...>>, front<list<Ts...>>, quote<detail::min_>>;

        /// An integral constant wrapper around the maximum of `Ts::type::value...`
        /// \ingroup math
        template <typename... Ts>
        using max = fold<pop_front<list<Ts...>>, front<list<Ts...>>, quote<detail::max_>>;

        namespace lazy
        {
            /// \sa 'meta::min'
            /// \ingroup lazy_math
            template <typename... Ts>
            using min = defer<min, Ts...>;

            /// \sa 'meta::max'
            /// \ingroup lazy_math
            template <typename... Ts>
            using max = defer<max, Ts...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // empty
        /// An Boolean integral constant wrapper around \c true if \p List is an empty
        /// type list; \c
        /// false, otherwise.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup list
        template <typename List>
        using empty = bool_<0 == size<List>::type::value>;

        namespace lazy
        {
            /// \sa 'meta::empty'
            /// \ingroup lazy_list
            template <typename List>
            using empty = defer<empty, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // pair
        /// A list with exactly two elements
        /// \ingroup list
        template <typename F, typename S>
        using pair = list<F, S>;

        /// Retrieve the first element of the \c pair \p Pair
        /// \ingroup list
        template <typename Pair>
        using first = front<Pair>;

        /// Retrieve the first element of the \c pair \p Pair
        /// \ingroup list
        template <typename Pair>
        using second = front<pop_front<Pair>>;

        namespace lazy
        {
            /// \sa 'meta::first'
            /// \ingroup lazy_list
            template <typename Pair>
            using first = defer<first, Pair>;

            /// \sa 'meta::second'
            /// \ingroup lazy_list
            template <typename Pair>
            using second = defer<second, Pair>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // find_index
        /// \cond
        namespace detail
        {
            // With thanks to Peter Dimov:
            constexpr std::size_t find_index_i_(bool const *const first, bool const *const last,
                                                std::size_t N = 0)
            {
                return first == last ? npos::value
                                     : *first ? N : find_index_i_(first + 1, last, N + 1);
            }

            template <typename List, typename T>
            struct find_index_
            {
            };

            template <typename V>
            struct find_index_<list<>, V>
            {
                using type = npos;
            };

            template <typename... T, typename V>
            struct find_index_<list<T...>, V>
            {
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(T)] = {std::is_same<T, V>::value...};
                using type = size_t<find_index_i_(s_v, s_v + sizeof...(T))>;
            };
        } // namespace detail
        /// \endcond

        /// Finds the index of the first occurrence of the type \p T within the list \p
        /// List.
        /// Returns `#meta::npos` if the type \p T was not found.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        /// \sa `meta::npos`
        template <typename List, typename T>
        using find_index = _t<detail::find_index_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::find_index'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using find_index = defer<find_index, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse_find_index
        /// \cond
        namespace detail
        {
            // With thanks to Peter Dimov:
            constexpr std::size_t reverse_find_index_i_(bool const *const first,
                                                        bool const *const last, std::size_t N)
            {
                return first == last
                           ? npos::value
                           : *(last - 1) ? N - 1 : reverse_find_index_i_(first, last - 1, N - 1);
            }

            template <typename List, typename T>
            struct reverse_find_index_
            {
            };

            template <typename V>
            struct reverse_find_index_<list<>, V>
            {
                using type = npos;
            };

            template <typename... T, typename V>
            struct reverse_find_index_<list<T...>, V>
            {
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(T)] = {std::is_same<T, V>::value...};
                using type = size_t<reverse_find_index_i_(s_v, s_v + sizeof...(T), sizeof...(T))>;
            };
        } // namespace detail
        /// \endcond

        /// Finds the index of the last occurrence of the type \p T within the list \p
        /// List. Returns
        /// `#meta::npos` if the type \p T was not found.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        /// \sa `#meta::npos`
        template <typename List, typename T>
        using reverse_find_index = _t<detail::reverse_find_index_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::reverse_find_index'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using reverse_find_index = defer<reverse_find_index, List, T>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // find
        /// Return the tail of the list \p List starting at the first occurrence of \p
        /// T, if any
        /// such element exists; the empty list, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using find = drop<List, min<find_index<List, T>, size<List>>>;

        namespace lazy
        {
            /// \sa 'meta::find'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using find = defer<find, List, T>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // reverse_find
        /// Return the tail of the list \p List starting at the last occurrence of \p T,
        /// if any such
        /// element exists; the empty list, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using reverse_find = drop<List, min<reverse_find_index<List, T>, size<List>>>;

        namespace lazy
        {
            /// \sa 'meta::rfind'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using reverse_find = defer<reverse_find, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // find_if
        /// \cond
        namespace detail
        {
            constexpr bool const *find_if_i_(bool const *const begin, bool const *const end)
            {
                return begin == end || *begin ? begin : find_if_i_(begin + 1, end);
            }

            template <typename List, typename Fun, typename = void>
            struct find_if_
            {
            };

            template <typename Fun>
            struct find_if_<list<>, Fun>
            {
                using type = list<>;
            };

            template <typename... List, typename Fun>
            struct find_if_<list<List...>, Fun,
                            void_<integer_sequence<bool, bool(invoke<Fun, List>::type::value)...>>>
            {
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {invoke<Fun, List>::type::value...};
                using type =
                    drop_c<list<List...>, detail::find_if_i_(s_v, s_v + sizeof...(List)) - s_v>;
            };
        } // namespace detail
        /// \endcond

        /// Return the tail of the list \p List starting at the first element `A` such
        /// that
        /// `invoke<Fun, A>::%value` is \c true, if any such element exists; the empty
        /// list,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename Fun>
        using find_if = _t<detail::find_if_<List, Fun>>;

        namespace lazy
        {
            /// \sa 'meta::find_if'
            /// \ingroup lazy_query
            template <typename List, typename Fun>
            using find_if = defer<find_if, List, Fun>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // reverse_find_if
        /// \cond
        namespace detail
        {
            constexpr bool const *reverse_find_if_i_(bool const *const begin, bool const *const pos,
                                                     bool const *const end)
            {
                return begin == pos
                           ? end
                           : *(pos - 1) ? pos - 1 : reverse_find_if_i_(begin, pos - 1, end);
            }

            template <typename List, typename Fun, typename = void>
            struct reverse_find_if_
            {
            };

            template <typename Fun>
            struct reverse_find_if_<list<>, Fun>
            {
                using type = list<>;
            };

            template <typename... List, typename Fun>
            struct reverse_find_if_<
                list<List...>, Fun,
                void_<integer_sequence<bool, bool(invoke<Fun, List>::type::value)...>>>
            {
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {invoke<Fun, List>::type::value...};
                using type =
                  drop_c<list<List...>, detail::reverse_find_if_i_(s_v, s_v + sizeof...(List),
                                                                   s_v + sizeof...(List)) - s_v>;
            };
        }
        /// \endcond

        /// Return the tail of the list \p List starting at the last element `A` such
        /// that
        /// `invoke<Fun, A>::%value` is \c true, if any such element exists; the empty
        /// list,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename Fun>
        using reverse_find_if = _t<detail::reverse_find_if_<List, Fun>>;

        namespace lazy
        {
            /// \sa 'meta::rfind_if'
            /// \ingroup lazy_query
            template <typename List, typename Fun>
            using reverse_find_if = defer<reverse_find_if, List, Fun>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // replace
        /// \cond
        namespace detail
        {
            template <typename List, typename T, typename U>
            struct replace_
            {
            };

            template <typename... List, typename T, typename U>
            struct replace_<list<List...>, T, U>
            {
                using type = list<if_<std::is_same<T, List>, U, List>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list where all instances of type \p T have been
        /// replaced with
        /// \p U.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename T, typename U>
        using replace = _t<detail::replace_<List, T, U>>;

        namespace lazy
        {
            /// \sa 'meta::replace'
            /// \ingroup lazy_transformation
            template <typename List, typename T, typename U>
            using replace = defer<replace, T, U>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // replace_if
        /// \cond
        namespace detail
        {
            template <typename List, typename C, typename U, typename = void>
            struct replace_if_
            {
            };

            template <typename... List, typename C, typename U>
            struct replace_if_<list<List...>, C, U,
                               void_<integer_sequence<bool, bool(invoke<C, List>::type::value)...>>>
            {
                using type = list<if_<invoke<C, List>, U, List>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list where all elements \c A of the list \p List for
        /// which
        /// `invoke<C,A>::%value` is \c true have been replaced with \p U.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename C, typename U>
        using replace_if = _t<detail::replace_if_<List, C, U>>;

        namespace lazy
        {
            /// \sa 'meta::replace_if'
            /// \ingroup lazy_transformation
            template <typename List, typename C, typename U>
            using replace_if = defer<replace_if, C, U>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // count
        namespace detail
        {
            constexpr std::size_t count_i_(bool const *const begin, bool const *const end,
                                           std::size_t n)
            {
                return begin == end ? n : detail::count_i_(begin + 1, end, n + *begin);
            }

            template <typename List, typename T, typename = void>
            struct count_
            {
            };

            template <typename T>
            struct count_<list<>, T>
            {
                using type = meta::size_t<0>;
            };

            template <typename... List, typename T>
            struct count_<list<List...>, T>
            {
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {std::is_same<T, List>::value...};
                using type = meta::size_t<detail::count_i_(s_v, s_v + sizeof...(List), 0u)>;
            };
        }

        /// Count the number of times a type \p T appears in the list \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using count = _t<detail::count_<List, T>>;

        namespace lazy
        {
            /// \sa `meta::count`
            /// \ingroup lazy_query
            template <typename List, typename T>
            using count = defer<count, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // count_if
        namespace detail
        {
            template <typename List, typename Fn, typename = void>
            struct count_if_
            {
            };

            template <typename Fn>
            struct count_if_<list<>, Fn>
            {
                using type = meta::size_t<0>;
            };

            template <typename... List, typename Fn>
            struct count_if_<list<List...>, Fn,
                             void_<integer_sequence<bool, bool(invoke<Fn, List>::type::value)...>>>
            {
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {invoke<Fn, List>::type::value...};
                using type = meta::size_t<detail::count_i_(s_v, s_v + sizeof...(List), 0u)>;
            };
        }

        /// Count the number of times the predicate \p Fn evaluates to true for all the
        /// elements in
        /// the list \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename Fn>
        using count_if = _t<detail::count_if_<List, Fn>>;

        namespace lazy
        {
            /// \sa `meta::count_if`
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using count_if = defer<count_if, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // filter
        /// \cond
        namespace detail
        {
            template <typename Pred>
            struct filter_
            {
                template <typename A>
                using invoke = if_c<invoke<Pred, A>::type::value, list<A>, list<>>;
            };
        } // namespace detail
        /// \endcond

        /// Returns a new meta::list where only those elements of \p List that satisfy
        /// the
        /// Callable \p Pred such that `invoke<Pred,A>::%value` is \c true are present.
        /// That is, those elements that don't satisfy the \p Pred are "removed".
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename Pred>
        using filter = join<transform<List, detail::filter_<Pred>>>;

        namespace lazy
        {
            /// \sa 'meta::filter'
            /// \ingroup lazy_transformation
            template <typename List, typename Pred>
            using filter = defer<filter, List, Pred>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // static_const
        ///\cond
        namespace detail
        {
            template <typename T>
            struct static_const
            {
                static constexpr T value{};
            };

            // Avoid potential ODR violations with global objects:
            template <typename T>
            constexpr T static_const<T>::value;
        } // namespace detail

        ///\endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // for_each
        /// \cond
        namespace detail
        {
            struct for_each_fn
            {
                template <class UnaryFunction, class... Args>
                constexpr auto operator()(list<Args...>, UnaryFunction f) const -> UnaryFunction
                {
                    return (void)std::initializer_list<int>{((void)f(Args{}), 0)...}, f;
                }
            };
        } // namespace detail
        /// \endcond

        ///\cond
        namespace
        {
            /// \endcond

            /// `for_each(List, UnaryFunction)` calls the \p UnaryFunction for each
            /// argument in the \p List.
            /// \ingroup runtime
            constexpr auto &&for_each = detail::static_const<detail::for_each_fn>::value;

            /// \cond
        }
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // transpose
        /// Given a list of lists of types \p ListOfLists, transpose the elements from
        /// the lists.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using transpose = fold<ListOfLists, repeat_n<size<front<ListOfLists>>, list<>>,
                               bind_back<quote<transform>, quote<push_back>>>;

        namespace lazy
        {
            /// \sa 'meta::transpose'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using transpose = defer<transpose, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // zip_with
        /// Given a list of lists of types \p ListOfLists and a Callable \p Fun,
        /// construct
        /// a new list by calling \p Fun with the elements from the lists pairwise.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <typename Fun, typename ListOfLists>
        using zip_with = transform<transpose<ListOfLists>, uncurry<Fun>>;

        namespace lazy
        {
            /// \sa 'meta::zip_with'
            /// \ingroup lazy_transformation
            template <typename Fun, typename ListOfLists>
            using zip_with = defer<zip_with, Fun, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // zip
        /// Given a list of lists of types \p ListOfLists, construct a new list by
        /// grouping the
        /// elements from the lists pairwise into `meta::list`s.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$
        /// is the size of the inner lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using zip = transpose<ListOfLists>;

        namespace lazy
        {
            /// \sa 'meta::zip'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using zip = defer<zip, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // as_list
        /// \cond
        namespace detail
        {
            template <typename T>
            using uncvref_t = _t<std::remove_cv<_t<std::remove_reference<T>>>>;

            // Indirection here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename Sequence>
            struct as_list_ : lazy::invoke<uncurry<curry<quote_trait<id>>>, uncvref_t<Sequence>>
            {
            };
        } // namespace detail
        /// \endcond

        /// Turn a type into an instance of \c meta::list in a way determined by
        /// \c meta::invoke.
        /// \ingroup list
        template <typename Sequence>
        using as_list = _t<detail::as_list_<Sequence>>;

        namespace lazy
        {
            /// \sa 'meta::as_list'
            /// \ingroup lazy_list
            template <typename Sequence>
            using as_list = defer<as_list, Sequence>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse
        /// \cond
        namespace detail
        {
            template <typename List, typename State = list<>>
            struct reverse_ : lazy::fold<List, State, quote<push_front>>
            {
            };

            template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
                      typename T6, typename T7, typename T8, typename T9, typename... Ts,
                      typename... Us>
            struct reverse_<list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, Ts...>, list<Us...>>
                : reverse_<list<Ts...>, list<T9, T8, T7, T6, T5, T4, T3, T2, T1, T0, Us...>>
            {
            };
        }
        /// \endcond

        /// Return a new \c meta::list by reversing the elements in the list \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List>
        using reverse = _t<detail::reverse_<List>>;

        namespace lazy
        {
            /// \sa 'meta::reverse'
            /// \ingroup lazy_transformation
            template <typename List>
            using reverse = defer<reverse, List>;
        }

        /// Logically negate the result of Callable \p F.
        /// \ingroup trait
        template <typename F>
        using not_fn = compose<quote<not_>, F>;

        namespace lazy
        {
            /// \sa 'meta::not_fn'
            /// \ingroup lazy_trait
            template <typename F>
            using not_fn = defer<not_fn, F>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // all_of
        /// A Boolean integral constant wrapper around \c true if `invoke<F, A>::%value`
        /// is \c true
        /// for all elements \c A in \c meta::list \p List; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename F>
        using all_of = empty<find_if<List, not_fn<F>>>;

        namespace lazy
        {
            /// \sa 'meta::all_of'
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using all_of = defer<all_of, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // any_of
        /// A Boolean integral constant wrapper around \c true if `invoke<F, A>::%value`
        /// is
        /// \c true for any element \c A in \c meta::list \p List; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename F>
        using any_of = not_<empty<find_if<List, F>>>;

        namespace lazy
        {
            /// \sa 'meta::any_of'
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using any_of = defer<any_of, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // none_of
        /// A Boolean integral constant wrapper around \c true if `invoke<F, A>::%value`
        /// is
        /// \c false for all elements \c A in \c meta::list \p List; \c false,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename F>
        using none_of = empty<find_if<List, F>>;

        namespace lazy
        {
            /// \sa 'meta::none_of'
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using none_of = defer<none_of, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // in
        /// A Boolean integral constant wrapper around \c true if there is at least one
        /// occurrence
        /// of \p T in \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using in = not_<empty<find<List, T>>>;

        namespace lazy
        {
            /// \sa 'meta::in'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using in = defer<in, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // inherit
        /// \cond
        namespace detail
        {
            template <typename List>
            struct inherit_
            {
            };

            template <typename... List>
            struct inherit_<list<List...>> : List...
            {
                using type = inherit_;
            };
        }
        /// \endcond

        /// A type that inherits from all the types in the list
        /// \pre The types in the list must be unique
        /// \pre All the types in the list must be non-final class types
        /// \ingroup datatype
        template <typename List>
        using inherit = meta::_t<detail::inherit_<List>>;

        namespace lazy
        {
            /// \sa 'meta::inherit'
            /// \ingroup lazy_datatype
            template <typename List>
            using inherit = defer<inherit, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // set
        // Used to improve the performance of \c meta::unique.
        /// \cond
        namespace detail
        {
            template <typename Set, typename T>
            struct in_
            {
            };

            template <typename... Set, typename T>
            struct in_<list<Set...>, T> : std::is_base_of<id<T>, inherit<list<id<Set>...>>>
            {
            };

            template <typename Set, typename T>
            struct insert_back_
            {
            };

            template <typename... Set, typename T>
            struct insert_back_<list<Set...>, T>
            {
                using type = if_<in_<list<Set...>, T>, list<Set...>, list<Set..., T>>;
            };
        } // namespace detail
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // unique
        /// Return a new \c meta::list where all duplicate elements have been removed.
        /// \par Complexity
        /// \f$ O(N^2) \f$.
        /// \ingroup transformation
        template <typename List>
        using unique = fold<List, list<>, quote_trait<detail::insert_back_>>;

        namespace lazy
        {
            /// \sa 'meta::unique'
            /// \ingroup lazy_transformation
            template <typename List>
            using unique = defer<unique, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // partition
        /// \cond
        namespace detail
        {
            template <typename Pred>
            struct partition_
            {
                template <typename, typename, typename = void>
                struct impl
                {
                };
                template <typename... Yes, typename... No, typename A>
                struct impl<pair<list<Yes...>, list<No...>>, A,
                            void_<bool_<invoke<Pred, A>::type::value>>>
                {
                    using type = if_<invoke<Pred, A>, pair<list<Yes..., A>, list<No...>>,
                                     pair<list<Yes...>, list<No..., A>>>;
                };
                template <typename State, typename A>
                using invoke = _t<impl<State, A>>;
            };
        } // namespace detail
        /// \endcond

        /// Returns a pair of lists, where the elements of \p List that satisfy the
        /// Callable \p Pred such that `invoke<Pred,A>::%value` is \c true are present
        /// in the
        /// first list and the rest are in the second.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename Pred>
        using partition = fold<List, pair<list<>, list<>>, detail::partition_<Pred>>;

        namespace lazy
        {
            /// \sa 'meta::partition'
            /// \ingroup lazy_transformation
            template <typename List, typename Pred>
            using partition = defer<partition, List, Pred>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // sort
        /// \cond
        namespace detail
        {
            template <typename, typename, typename = void>
            struct sort_
            {
            };

            template <typename Pred>
            struct sort_<list<>, Pred>
            {
                using type = list<>;
            };

            template <typename A, typename Pred>
            struct sort_<list<A>, Pred>
            {
                using type = list<A>;
            };

            template <typename A, typename B, typename... List, typename Pred>
            struct sort_<
                list<A, B, List...>, Pred,
                void_<_t<sort_<first<partition<list<B, List...>, bind_back<Pred, A>>>, Pred>>>>
            {
                using P = partition<list<B, List...>, bind_back<Pred, A>>;
                using type = concat<_t<sort_<first<P>, Pred>>, list<A>, _t<sort_<second<P>, Pred>>>;
            };
        }
        /// \endcond

        // clang-format off
        /// Return a new \c meta::list that is sorted according to Callable predicate \p Pred.
        /// \par Complexity
        /// Expected: \f$ O(N log N) \f$
        /// Worst case: \f$ O(N^2) \f$.
        /// \code
        /// using L0 = list<char[5], char[3], char[2], char[6], char[1], char[5], char[10]>;
        /// using L1 = meta::sort<L0, lambda<_a, _b, lazy::less<lazy::sizeof_<_a>, lazy::sizeof_<_b>>>>;
        /// static_assert(std::is_same<L1, list<char[1], char[2], char[3], char[5], char[5], char[6], char[10]>>::value, "");
        /// \endcode
        /// \ingroup transformation
        // clang-format on
        template <typename List, typename Pred>
        using sort = _t<detail::sort_<List, Pred>>;

        namespace lazy
        {
            /// \sa 'meta::sort'
            /// \ingroup lazy_transformation
            template <typename List, typename Pred>
            using sort = defer<sort, List, Pred>;
        }

        ////////////////////////////////////////////////////////////////////////////
        // lambda_
        /// \cond
        namespace detail
        {
            template <typename T, int = 0>
            struct protect_;

            template <typename, int = 0>
            struct vararg_;

            template <typename T, int = 0>
            struct is_valid_;

            // Returns which branch to evaluate
            template <typename If, typename... Ts>
            using lazy_if_ = lazy::_t<defer<_if_, list<If, protect_<Ts>...>>>;

            template <typename A, typename T, typename F, typename Ts>
            struct subst1_
            {
                using type = list<list<T>>;
            };
            template <typename T, typename F, typename Ts>
            struct subst1_<F, T, F, Ts>
            {
                using type = list<>;
            };
            template <typename A, typename T, typename F, typename Ts>
            struct subst1_<vararg_<A>, T, F, Ts>
            {
                using type = list<Ts>;
            };

            template <typename As, typename Ts>
            using substitutions_ = push_back<
                join<transform<
                    concat<As, repeat_n_c<size<Ts>{} + 2 - size<As>{}, back<As>>>,
                    concat<Ts, repeat_n_c<2, back<As>>>,
                    bind_back<quote_trait<subst1_>, back<As>, drop_c<Ts, size<As>{} - 2>>>>,
                list<back<As>>>;

            template <typename As, typename Ts>
            using substitutions =
                invoke<if_c<(size<Ts>{} + 2 >= size<As>{}), quote<substitutions_>>, As, Ts>;

            template <typename T>
            struct is_vararg_ : std::false_type
            {
            };
            template <typename T>
            struct is_vararg_<vararg_<T>> : std::true_type
            {
            };

            template <typename Tags>
            using is_variadic_ = is_vararg_<at<push_front<Tags, void>, dec<size<Tags>>>>;

            template <typename Tags, bool IsVariadic = is_variadic_<Tags>::value>
            struct lambda_;

            // Non-variadic lambda implementation
            template <typename... As>
            struct lambda_<list<As...>, false>
            {
            private:
                static constexpr std::size_t arity = sizeof...(As)-1;
                using Tags = list<As...>; // Includes the lambda body as the last arg!
                using F = back<Tags>;
                template <typename T, typename Args>
                struct impl;
                template <typename T, typename Args>
                using lazy_impl_ = lazy::_t<defer<impl, T, protect_<Args>>>;
                template <typename, typename, typename = void>
                struct subst_
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct subst_<defer<C, Ts...>, Args, void_<C<_t<impl<Ts, Args>>...>>>
                {
                    using type = C<_t<impl<Ts, Args>>...>;
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct subst_<defer_i<T, C, Is...>, Args, void_<C<Is...>>>
                {
                    using type = C<Is...>;
                };
                template <typename T, typename Args>
                struct impl : if_c<(reverse_find_index<Tags, T>() != npos()),
                                   lazy::at<Args, reverse_find_index<Tags, T>>, id<T>>
                {
                };
                template <typename T, typename Args>
                struct impl<protect_<T>, Args>
                {
                    using type = T;
                };
                template <typename T, typename Args>
                struct impl<is_valid_<T>, Args>
                {
                    using type = is_trait<impl<T, Args>>;
                };
                template <typename If, typename... Ts, typename Args>
                struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                    : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                {
                };
                template <typename Bool, typename... Ts, typename Args>
                struct impl<defer<and_, Bool, Ts...>, Args> // Short-circuit and_
                    : impl<lazy_impl_<lazy_if_<Bool, lazy::and_<Ts...>, protect_<std::false_type>>,
                                      Args>,
                           Args>
                {
                };
                template <typename Bool, typename... Ts, typename Args>
                struct impl<defer<or_, Bool, Ts...>, Args> // Short-circuit or_
                    : impl<lazy_impl_<lazy_if_<Bool, protect_<std::true_type>, lazy::or_<Ts...>>,
                                      Args>,
                           Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename... Ts, typename Args>
                struct impl<lambda_<list<Ts...>, false>, Args>
                {
                    using type = compose<uncurry<lambda_<list<As..., Ts...>, false>>,
                                         curry<bind_front<quote<concat>, Args>>>;
                };
                template <typename... Bs, typename Args>
                struct impl<lambda_<list<Bs...>, true>, Args>
                {
                    using type = compose<typename lambda_<list<As..., Bs...>, true>::thunk,
                                         bind_front<quote<concat>, transform<Args, quote<list>>>,
                                         curry<bind_front<quote<substitutions>, list<Bs...>>>>;
                };

            public:
                template <typename... Ts>
                using invoke = _t<if_c<sizeof...(Ts) == arity, impl<F, list<Ts..., F>>>>;
            };

            // Lambda with variadic placeholder (broken out due to less efficient
            // compile-time
            // resource usage)
            template <typename... As>
            struct lambda_<list<As...>, true>
            {
            private:
                template <typename T, bool IsVar>
                friend struct lambda_;
                using Tags = list<As...>; // Includes the lambda body as the last arg!
                template <typename T, typename Args>
                struct impl;
                template <typename Args>
                using eval_impl_ = bind_back<quote_trait<impl>, Args>;
                template <typename T, typename Args>
                using lazy_impl_ = lazy::_t<defer<impl, T, protect_<Args>>>;
                template <template <typename...> class C, typename Args, typename Ts>
                using try_subst_ = apply<quote<C>, join<transform<Ts, eval_impl_<Args>>>>;
                template <typename, typename, typename = void>
                struct subst_
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct subst_<defer<C, Ts...>, Args, void_<try_subst_<C, Args, list<Ts...>>>>
                {
                    using type = list<try_subst_<C, Args, list<Ts...>>>;
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct subst_<defer_i<T, C, Is...>, Args, void_<C<Is...>>>
                {
                    using type = list<C<Is...>>;
                };
                template <typename T, typename Args>
                struct impl : if_c<(reverse_find_index<Tags, T>() != npos()),
                                   lazy::at<Args, reverse_find_index<Tags, T>>, id<list<T>>>
                {
                };
                template <typename T, typename Args>
                struct impl<protect_<T>, Args>
                {
                    using type = list<T>;
                };
                template <typename T, typename Args>
                struct impl<is_valid_<T>, Args>
                {
                    using type = list<is_trait<impl<T, Args>>>;
                };
                template <typename If, typename... Ts, typename Args>
                struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                    : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                {
                };
                template <typename Bool, typename... Ts, typename Args>
                struct impl<defer<and_, Bool, Ts...>, Args> // Short-circuit and_
                    : impl<lazy_impl_<lazy_if_<Bool, lazy::and_<Ts...>, protect_<std::false_type>>,
                                      Args>,
                           Args>
                {
                };
                template <typename Bool, typename... Ts, typename Args>
                struct impl<defer<or_, Bool, Ts...>, Args> // Short-circuit or_
                    : impl<lazy_impl_<lazy_if_<Bool, protect_<std::true_type>, lazy::or_<Ts...>>,
                                      Args>,
                           Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename... Bs, bool IsVar, typename Args>
                struct impl<lambda_<list<Bs...>, IsVar>, Args>
                {
                    using type =
                        list<compose<typename lambda_<list<As..., Bs...>, true>::thunk,
                                     bind_front<quote<concat>, Args>,
                                     curry<bind_front<quote<substitutions>, list<Bs...>>>>>;
                };
                struct thunk
                {
                    template <typename S, typename R = _t<impl<back<Tags>, S>>>
                    using invoke = if_c<size<R>{} == 1, front<R>>;
                };

            public:
                template <typename... Ts>
                using invoke = invoke<thunk, substitutions<Tags, list<Ts...>>>;
            };
        }
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // lambda
        /// For creating anonymous Callables.
        /// \code
        /// using L = lambda<_a, _b, std::pair<_b, std::pair<_a, _a>>>;
        /// using P = invoke<L, int, short>;
        /// static_assert(std::is_same<P, std::pair<short, std::pair<int, int>>>::value,
        /// "");
        /// \endcode
        /// \ingroup trait
        template <typename... Ts>
        using lambda = if_c<(sizeof...(Ts) > 0), detail::lambda_<list<Ts...>>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // is_valid
        /// For testing whether a deferred computation will succeed in a \c let or a \c
        /// lambda.
        /// \ingroup trait
        template <typename T>
        using is_valid = detail::is_valid_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // vararg
        /// For defining variadic placeholders.
        template <typename T>
        using vararg = detail::vararg_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // protect
        /// For preventing the evaluation of a nested `defer`ed computation in a \c let
        /// or
        /// \c lambda expression.
        template <typename T>
        using protect = detail::protect_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // var
        /// For use when defining local variables in \c meta::let expressions
        /// \sa `meta::let`
        template <typename Tag, typename Value>
        struct var;

        /// \cond
        namespace detail
        {
            template <typename... As>
            struct let_
            {
            };
            template <typename Fn>
            struct let_<Fn>
            {
                using type = lazy::invoke<lambda<Fn>>;
            };
            template <typename Tag, typename Value, typename... Rest>
            struct let_<var<Tag, Value>, Rest...>
            {
                using type = lazy::invoke<lambda<Tag, _t<let_<Rest...>>>, Value>;
            };
        }
        /// \endcond

        /// A lexically scoped expression with local variables.
        ///
        /// \code
        /// template<typename T, typename List>
        /// using find_index_ = let<
        ///     var<_a, List>,
        ///     var<_b, lazy::find<_a, T>>,
        ///     lazy::if_<
        ///         std::is_same<_b, list<>>,
        ///         meta::npos,
        ///         lazy::minus<lazy::size<_a>, lazy::size<_b>>>>;
        /// static_assert(find_index_<int, list<short, int, float>>{} == 1, "");
        /// static_assert(find_index_<double, list<short, int, float>>{} ==
        /// meta::npos{}, "");
        /// \endcode
        /// \ingroup trait
        template <typename... As>
        using let = _t<_t<detail::let_<As...>>>;

        namespace lazy
        {
            /// \sa `meta::let`
            /// \ingroup lazy_trait
            template <typename... As>
            using let = defer<let, As...>;
        }

        // Some argument placeholders for use in \c lambda expressions.
        /// \ingroup trait
        inline namespace placeholders
        {
            // regular placeholders:
            struct _a;
            struct _b;
            struct _c;
            struct _d;
            struct _e;
            struct _f;
            struct _g;
            struct _h;
            struct _i;

            // variadic placeholders:
            using _args = vararg<void>;
            using _args_a = vararg<_a>;
            using _args_b = vararg<_b>;
            using _args_c = vararg<_c>;
        } // namespace placeholders

        ///////////////////////////////////////////////////////////////////////////////////////////
        // cartesian_product
        /// \cond
        namespace detail
        {
            template <typename M2, typename M>
            struct cartesian_product_fn
            {
                template <typename X>
                struct lambda0
                {
                    template <typename Xs>
                    using lambda1 = list<push_front<Xs, X>>;
                    using type = join<transform<M2, quote<lambda1>>>;
                };
                using type = join<transform<M, quote_trait<lambda0>>>;
            };
        } // namespace detail
        /// \endcond

        /// Given a list of lists \p ListOfLists, return a new list of lists that is the
        /// Cartesian
        /// Product. Like the `sequence` function from the Haskell Prelude.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using cartesian_product =
            reverse_fold<ListOfLists, list<list<>>, quote_trait<detail::cartesian_product_fn>>;

        namespace lazy
        {
            /// \sa 'meta::cartesian_product'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using cartesian_product = defer<cartesian_product, ListOfLists>;
        }

        /// \cond
        ///////////////////////////////////////////////////////////////////////////////////////////
        // add_const_if
        template <typename If>
        using add_const_if = if_<If, quote_trait<std::add_const>, quote_trait<id>>;

        template <bool If>
        using add_const_if_c = if_c<If, quote_trait<std::add_const>, quote_trait<id>>;
        /// \endcond

        /// \cond
        namespace detail
        {
            template <typename State, typename Ch>
            using atoi_ = if_c<(Ch::value >= '0' && Ch::value <= '9'),
                               std::integral_constant<typename State::value_type,
                                                      State::value * 10 + (Ch::value - '0')>>;
        }
        /// \endcond

        inline namespace literals
        {
            /// A user-defined literal that generates objects of type \c meta::size_t.
            /// \ingroup integral
            template <char... Chs>
            constexpr fold<list<char_<Chs>...>, meta::size_t<0>, quote<detail::atoi_>>
                operator"" _z()
            {
                return {};
            }
        }
        /// \cond
    } // namespace v1
      /// \endcond
} // namespace meta

/// \cond
#if defined(__clang__) && defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 1101

_LIBCPP_BEGIN_NAMESPACE_STD
template <class>
class _LIBCPP_TYPE_VIS_ONLY allocator;
template <class, class>
struct _LIBCPP_TYPE_VIS_ONLY pair;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY hash;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY less;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY equal_to;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY char_traits;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY list;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY forward_list;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY vector;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY deque;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY basic_string;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY map;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY multimap;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY set;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY multiset;
template <class, class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_map;
template <class, class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_multimap;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_set;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_multiset;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY queue;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY priority_queue;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY stack;
_LIBCPP_END_NAMESPACE_STD

namespace meta
{
    inline namespace v1
    {
        namespace detail
        {
            template <typename T, typename A = std::allocator<T>>
            using std_list = std::list<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_forward_list = std::forward_list<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_vector = std::vector<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_deque = std::deque<T, A>;
            template <typename T, typename C = std::char_traits<T>, typename A = std::allocator<T>>
            using std_basic_string = std::basic_string<T, C, A>;
            template <typename K, typename V, typename C = std::less<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_map = std::map<K, V, C, A>;
            template <typename K, typename V, typename C = std::less<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_multimap = std::multimap<K, V, C, A>;
            template <typename K, typename C = std::less<K>, typename A = std::allocator<K>>
            using std_set = std::set<K, C, A>;
            template <typename K, typename C = std::less<K>, typename A = std::allocator<K>>
            using std_multiset = std::multiset<K, C, A>;
            template <typename K, typename V, typename H = std::hash<K>,
                      typename C = std::equal_to<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_unordered_map = std::unordered_map<K, V, H, C, A>;
            template <typename K, typename V, typename H = std::hash<K>,
                      typename C = std::equal_to<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_unordered_multimap = std::unordered_multimap<K, V, H, C, A>;
            template <typename K, typename H = std::hash<K>, typename C = std::equal_to<K>,
                      typename A = std::allocator<K>>
            using std_unordered_set = std::unordered_set<K, H, C, A>;
            template <typename K, typename H = std::hash<K>, typename C = std::equal_to<K>,
                      typename A = std::allocator<K>>
            using std_unordered_multiset = std::unordered_multiset<K, H, C, A>;
            template <typename T, typename C = std_deque<T>>
            using std_queue = std::queue<T, C>;
            template <typename T, typename C = std_vector<T>,
                      class D = std::less<typename C::value_type>>
            using std_priority_queue = std::priority_queue<T, C, D>;
            template <typename T, typename C = std_deque<T>>
            using std_stack = std::stack<T, C>;
        }

        template <>
        struct quote<::std::list> : quote<detail::std_list>
        {
        };
        template <>
        struct quote<::std::deque> : quote<detail::std_deque>
        {
        };
        template <>
        struct quote<::std::forward_list> : quote<detail::std_forward_list>
        {
        };
        template <>
        struct quote<::std::vector> : quote<detail::std_vector>
        {
        };
        template <>
        struct quote<::std::basic_string> : quote<detail::std_basic_string>
        {
        };
        template <>
        struct quote<::std::map> : quote<detail::std_map>
        {
        };
        template <>
        struct quote<::std::multimap> : quote<detail::std_multimap>
        {
        };
        template <>
        struct quote<::std::set> : quote<detail::std_set>
        {
        };
        template <>
        struct quote<::std::multiset> : quote<detail::std_multiset>
        {
        };
        template <>
        struct quote<::std::unordered_map> : quote<detail::std_unordered_map>
        {
        };
        template <>
        struct quote<::std::unordered_multimap> : quote<detail::std_unordered_multimap>
        {
        };
        template <>
        struct quote<::std::unordered_set> : quote<detail::std_unordered_set>
        {
        };
        template <>
        struct quote<::std::unordered_multiset> : quote<detail::std_unordered_multiset>
        {
        };
        template <>
        struct quote<::std::queue> : quote<detail::std_queue>
        {
        };
        template <>
        struct quote<::std::priority_queue> : quote<detail::std_priority_queue>
        {
        };
        template <>
        struct quote<::std::stack> : quote<detail::std_stack>
        {
        };
    }
}

#endif
/// \endcond

#if defined(__clang__)
#pragma GCC diagnostic pop
#endif
#endif
// clang-format off
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

// disable buggy compatibility warning about "requires" and "concept" being
// C++20 keywords.
#if defined(__clang__)
#define PUSHMI_PP_IGNORE_CXX2A_COMPAT_BEGIN \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"") \
    _Pragma("GCC diagnostic ignored \"-Wpragmas\"") \
    _Pragma("GCC diagnostic ignored \"-Wc++2a-compat\"") \
    _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"") \
    /**/
#define PUSHMI_PP_IGNORE_CXX2A_COMPAT_END \
    _Pragma("GCC diagnostic pop")
#else
#define PUSHMI_PP_IGNORE_CXX2A_COMPAT_BEGIN
#define PUSHMI_PP_IGNORE_CXX2A_COMPAT_END
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunknown-pragmas"
// #pragma GCC diagnostic ignored "-Wpragmas"
// #pragma GCC diagnostic ignored "-Wc++2a-compat"
#endif

PUSHMI_PP_IGNORE_CXX2A_COMPAT_BEGIN

#if __cpp_inline_variables >= 201606
#define PUSHMI_INLINE_VAR inline
#else
#define PUSHMI_INLINE_VAR
#endif

#ifdef __clang__
#define PUSHMI_PP_IS_SAME(A, B) __is_same(A, B)
#elif defined(__GNUC__) && __GNUC__ >= 6
#define PUSHMI_PP_IS_SAME(A, B) __is_same_as(A, B)
#else
#define PUSHMI_PP_IS_SAME(A, B) std::is_same<A, B>::value
#endif

#if __COUNTER__ != __COUNTER__
#define PUSHMI_COUNTER __COUNTER__
#else
#define PUSHMI_COUNTER __LINE__
#endif

#define PUSHMI_PP_CHECK(...) PUSHMI_PP_CHECK_N(__VA_ARGS__, 0,)
#define PUSHMI_PP_CHECK_N(x, n, ...) n
#define PUSHMI_PP_PROBE(x) x, 1,

// PUSHMI_CXX_VA_OPT
#ifndef PUSHMI_CXX_VA_OPT
#if __cplusplus > 201703L
#define PUSHMI_CXX_VA_OPT_(...) PUSHMI_PP_CHECK(__VA_OPT__(,) 1)
#define PUSHMI_CXX_VA_OPT PUSHMI_CXX_VA_OPT_(~)
#else
#define PUSHMI_CXX_VA_OPT 0
#endif
#endif // PUSHMI_CXX_VA_OPT

#define PUSHMI_PP_CAT_(X, ...)  X ## __VA_ARGS__
#define PUSHMI_PP_CAT(X, ...)   PUSHMI_PP_CAT_(X, __VA_ARGS__)
#define PUSHMI_PP_CAT2_(X, ...) X ## __VA_ARGS__
#define PUSHMI_PP_CAT2(X, ...)  PUSHMI_PP_CAT2_(X, __VA_ARGS__)

#define PUSHMI_PP_EVAL(X, ...) X(__VA_ARGS__)
#define PUSHMI_PP_EVAL2(X, ...) X(__VA_ARGS__)
#define PUSHMI_PP_EVAL3(X, ...) X(__VA_ARGS__)
#define PUSHMI_PP_EVAL4(X, ...) X(__VA_ARGS__)

#define PUSHMI_PP_EXPAND(...) __VA_ARGS__
#define PUSHMI_PP_EAT(...)

#define PUSHMI_PP_IS_PAREN(x) PUSHMI_PP_CHECK(PUSHMI_PP_IS_PAREN_PROBE x)
#define PUSHMI_PP_IS_PAREN_PROBE(...) PUSHMI_PP_PROBE(~)

#define PUSHMI_PP_COUNT(...)                                                   \
    PUSHMI_PP_COUNT_(__VA_ARGS__,                                              \
        50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,           \
        30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,           \
        10,9,8,7,6,5,4,3,2,1,)                                                 \
        /**/
#define PUSHMI_PP_COUNT_(                                                      \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,                                   \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,                          \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,                          \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,                          \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, N, ...)                  \
    N                                                                          \
    /**/

#define PUSHMI_PP_IS_EQUAL(X,Y) \
    PUSHMI_PP_CHECK(PUSHMI_PP_CAT(PUSHMI_PP_CAT(PUSHMI_PP_IS_EQUAL_, X), Y))
#define PUSHMI_PP_IS_EQUAL_00 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_11 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_22 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_33 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_44 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_55 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_66 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_77 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_88 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_99 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1010 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1111 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1212 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1313 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1414 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1515 PUSHMI_PP_PROBE(~)
#define PUSHMI_PP_IS_EQUAL_1616 PUSHMI_PP_PROBE(~)

#define PUSHMI_PP_NOT(X) PUSHMI_PP_CAT(PUSHMI_PP_NOT_, X)
#define PUSHMI_PP_NOT_0 1
#define PUSHMI_PP_NOT_1 0

#define PUSHMI_PP_IIF(BIT) PUSHMI_PP_CAT_(PUSHMI_PP_IIF_, BIT)
#define PUSHMI_PP_IIF_0(TRUE, ...) __VA_ARGS__
#define PUSHMI_PP_IIF_1(TRUE, ...) TRUE
#define PUSHMI_PP_FIRST(X, ...) X
#define PUSHMI_PP_SECOND(X, ...) __VA_ARGS__

#define PUSHMI_PP_OR(X,Y) \
    PUSHMI_PP_NOT(PUSHMI_PP_CHECK(PUSHMI_PP_CAT(                               \
        PUSHMI_PP_CAT(PUSHMI_PP_NOR_, X), Y)))
#define PUSHMI_PP_NOR_00 PUSHMI_PP_PROBE(~)

#if PUSHMI_CXX_VA_OPT

#define PUSHMI_PP_IS_EMPTY_NON_FUNCTION(...) \
    PUSHMI_PP_CHECK(__VA_OPT__(, 0), 1)

#else // RANGES_VA_OPT

#define PUSHMI_PP_SPLIT(i, ...)                                                \
    PUSHMI_PP_CAT_(PUSHMI_PP_SPLIT_, i)(__VA_ARGS__)                           \
    /**/
#define PUSHMI_PP_SPLIT_0(a, ...) a
#define PUSHMI_PP_SPLIT_1(a, ...) __VA_ARGS__

#define PUSHMI_PP_IS_VARIADIC(...)                                             \
    PUSHMI_PP_SPLIT(                                                           \
        0,                                                                     \
        PUSHMI_PP_CAT(                                                         \
            PUSHMI_PP_IS_VARIADIC_R_,                                          \
            PUSHMI_PP_IS_VARIADIC_C __VA_ARGS__))                              \
    /**/
#define PUSHMI_PP_IS_VARIADIC_C(...) 1
#define PUSHMI_PP_IS_VARIADIC_R_1 1,
#define PUSHMI_PP_IS_VARIADIC_R_PUSHMI_PP_IS_VARIADIC_C 0,

// emptiness detection macro...

#define PUSHMI_PP_IS_EMPTY_NON_FUNCTION(...)                                   \
    PUSHMI_PP_IIF(PUSHMI_PP_IS_VARIADIC(__VA_ARGS__))                          \
    (0, PUSHMI_PP_IS_VARIADIC(PUSHMI_PP_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__()))\
    /**/
#define PUSHMI_PP_IS_EMPTY_NON_FUNCTION_C() ()

#endif // PUSHMI_PP_VA_OPT

#define PUSHMI_PP_EMPTY()
#define PUSHMI_PP_COMMA() ,
#define PUSHMI_PP_COMMA_IIF(X)                                                 \
    PUSHMI_PP_IIF(X)(PUSHMI_PP_EMPTY, PUSHMI_PP_COMMA)()

#define PUSHMI_CONCEPT_ASSERT(...)                                             \
    static_assert((bool) (__VA_ARGS__),                                        \
        "Concept assertion failed : " #__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// PUSHMI_CONCEPT_DEF
//   For defining concepts with a syntax symilar to C++20. For example:
//
//     PUSHMI_CONCEPT_DEF(
//         // The Assignable concept from the C++20
//         template(class T, class U)
//         concept Assignable,
//             requires (T t, U &&u) (
//                 t = (U &&) u,
//                 ::pushmi::concepts::requires_<Same<decltype(t = (U &&) u), T>>
//             ) &&
//             std::is_lvalue_reference<T>{}
//     );
#define PUSHMI_CONCEPT_DEF(DECL, ...)                                          \
    PUSHMI_PP_EVAL(                                                            \
        PUSHMI_PP_DECL_DEF,                                                    \
        PUSHMI_PP_CAT(PUSHMI_PP_DEF_DECL_, DECL),                              \
        __VA_ARGS__,                                                           \
        PUSHMI_PP_EAT)                                                         \
    /**/
#define PUSHMI_PP_DECL_DEF_NAME(...) __VA_ARGS__,
#define PUSHMI_PP_DECL_DEF(TPARAM, NAME, REQUIRES, ...)                        \
    PUSHMI_PP_CAT(PUSHMI_PP_DECL_DEF_, PUSHMI_PP_IS_PAREN(NAME))(              \
        TPARAM,                                                                \
        NAME,                                                                  \
        REQUIRES,                                                              \
        __VA_ARGS__)                                                           \
    /**/
// The defn is of the form:
//   template(class A, class B = void, class... Rest)
//   (concept Name)(A, B, Rest...),
//      // requirements...
#define PUSHMI_PP_DECL_DEF_1(TPARAM, NAME, REQUIRES, ...)                      \
    PUSHMI_PP_EVAL4(                                                           \
        PUSHMI_PP_DECL_DEF_IMPL,                                               \
        TPARAM,                                                                \
        PUSHMI_PP_DECL_DEF_NAME NAME,                                          \
        REQUIRES,                                                              \
        __VA_ARGS__)                                                           \
    /**/
// The defn is of the form:
//   template(class A, class B)
//   concept Name,
//      // requirements...
// Compute the template arguments (A, B) from the template introducer.
#define PUSHMI_PP_DECL_DEF_0(TPARAM, NAME, REQUIRES, ...)                      \
    PUSHMI_PP_DECL_DEF_IMPL(                                                   \
        TPARAM,                                                                \
        NAME,                                                                  \
        (PUSHMI_PP_CAT(PUSHMI_PP_AUX_, TPARAM)),                               \
        REQUIRES,                                                              \
        __VA_ARGS__)                                                           \
    /**/
// Expand the template definition into a struct and template alias like:
//    struct NameConcept {
//      template<class A, class B>
//      static auto _concept_requires_(/* args (optional)*/) ->
//          decltype(/*requirements...*/);
//      template<class A, class B>
//      static constexpr auto is_satisfied_by(int) ->
//          decltype(bool(&_concept_requires_<A,B>)) { return true; }
//      template<class A, class B>
//      static constexpr bool is_satisfied_by(long) { return false; }
//    };
//    template<class A, class B>
//    inline constexpr bool Name = NameConcept::is_satisfied_by<A, B>(0);
#define PUSHMI_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, REQUIRES, ...)             \
    struct PUSHMI_PP_CAT(PUSHMI_PP_CAT(PUSHMI_PP_DEF_, NAME), Concept) {       \
        using Concept =                                                        \
            PUSHMI_PP_CAT(PUSHMI_PP_CAT(PUSHMI_PP_DEF_, NAME), Concept);       \
        PUSHMI_PP_IGNORE_CXX2A_COMPAT_BEGIN                                    \
        PUSHMI_PP_CAT(PUSHMI_PP_DEF_, TPARAM)                                  \
        static auto _concept_requires_ PUSHMI_PP_EVAL2(                        \
            PUSHMI_PP_DEF_WRAP,                                                \
            PUSHMI_PP_CAT(PUSHMI_PP_DEF_, REQUIRES),                           \
            REQUIRES,                                                          \
            (__VA_ARGS__))(~) int>;                                            \
        PUSHMI_PP_IGNORE_CXX2A_COMPAT_END                                      \
        PUSHMI_PP_CAT(PUSHMI_PP_DEF_, TPARAM)                                  \
        static constexpr decltype(::pushmi::concepts::detail                   \
            ::gcc_bugs_bugs_bugs(&_concept_requires_<PUSHMI_PP_EXPAND ARGS>))  \
        is_satisfied_by(int) noexcept { return true; }                         \
        PUSHMI_PP_CAT(PUSHMI_PP_DEF_, TPARAM)                                  \
        static constexpr bool is_satisfied_by(long) noexcept { return false; } \
    };                                                                         \
    PUSHMI_PP_CAT(PUSHMI_PP_DEF_, TPARAM)                                      \
    PUSHMI_INLINE_VAR constexpr bool PUSHMI_PP_CAT(PUSHMI_PP_DEF_, NAME) =     \
        PUSHMI_PP_CAT(PUSHMI_PP_CAT(PUSHMI_PP_DEF_, NAME), Concept)            \
            ::is_satisfied_by<PUSHMI_PP_EXPAND ARGS>(0);                       \
    /**/

#define PUSHMI_PP_DEF_DECL_template(...)                                       \
    template(__VA_ARGS__),                                                     \
    /**/
#define PUSHMI_PP_DEF_template(...)                                            \
    template<__VA_ARGS__>                                                      \
    /**/
#define PUSHMI_PP_DEF_concept
#define PUSHMI_PP_DEF_class
#define PUSHMI_PP_DEF_typename
#define PUSHMI_PP_DEF_int
#define PUSHMI_PP_DEF_bool
#define PUSHMI_PP_DEF_size_t
#define PUSHMI_PP_DEF_unsigned
#define PUSHMI_PP_DEF_requires ~,
#define PUSHMI_PP_AUX_template(...)                                            \
    PUSHMI_PP_CAT2(                                                            \
        PUSHMI_PP_TPARAM_,                                                     \
        PUSHMI_PP_COUNT(__VA_ARGS__))(__VA_ARGS__)                             \
    /**/
#define PUSHMI_PP_TPARAM_1(_1)                                                 \
    PUSHMI_PP_CAT2(PUSHMI_PP_DEF_, _1)
#define PUSHMI_PP_TPARAM_2(_1, ...)                                            \
    PUSHMI_PP_CAT2(PUSHMI_PP_DEF_, _1), PUSHMI_PP_TPARAM_1(__VA_ARGS__)
#define PUSHMI_PP_TPARAM_3(_1, ...)                                            \
    PUSHMI_PP_CAT2(PUSHMI_PP_DEF_, _1), PUSHMI_PP_TPARAM_2(__VA_ARGS__)
#define PUSHMI_PP_TPARAM_4(_1, ...)                                            \
    PUSHMI_PP_CAT2(PUSHMI_PP_DEF_, _1), PUSHMI_PP_TPARAM_3(__VA_ARGS__)
#define PUSHMI_PP_TPARAM_5(_1, ...)                                            \
    PUSHMI_PP_CAT2(PUSHMI_PP_DEF_, _1), PUSHMI_PP_TPARAM_4(__VA_ARGS__)

#define PUSHMI_PP_DEF_WRAP(X, Y, ...)                                          \
    PUSHMI_PP_EVAL3(                                                           \
        PUSHMI_PP_CAT(PUSHMI_PP_DEF_WRAP_, PUSHMI_PP_COUNT(__VA_ARGS__)),      \
        X,                                                                     \
        Y,                                                                     \
        __VA_ARGS__)                                                           \
    /**/

// No requires expression:
#define PUSHMI_PP_DEF_WRAP_1(_, HEAD, TAIL)                                    \
    () -> std::enable_if_t<HEAD, PUSHMI_PP_EXPAND TAIL                         \
    /**/
// Requires expression:
#define PUSHMI_PP_DEF_WRAP_2(_a, HEAD, _b, TAIL)                               \
    PUSHMI_PP_CAT(PUSHMI_PP_DEF_REQUIRES_, PUSHMI_PP_IS_PAREN(HEAD)) HEAD,     \
    PUSHMI_PP_EXPAND TAIL                                                      \
    /**/
// Requires expression without a requirement parameter list:
#define PUSHMI_PP_DEF_REQUIRES_0                                               \
    () -> decltype(::pushmi::concepts::detail::requires_                       \
        PUSHMI_PP_DEF_REQUIRES_EXPRS                                           \
    /**/
// Requires expression with a requirement parameter list:
#define PUSHMI_PP_DEF_REQUIRES_1(...)                                          \
    (__VA_ARGS__) -> std::enable_if_t<::pushmi::concepts::detail::requires_    \
        PUSHMI_PP_DEF_REQUIRES_EXPRS                                           \
    /**/

#define  PUSHMI_PP_DEF_REQUIRES_EXPRS(...) \
    <decltype(__VA_ARGS__, void())>()\
    /**/

#define PUSHMI_TYPE_CONSTRAINT(X) class

#define PUSHMI_TEMPLATE(...)                                                   \
    template<__VA_ARGS__ PUSHMI_TEMPLATE_AUX_
#define PUSHMI_TEMPLATE_AUX_(...) ,                                            \
    int (*_pushmi_concept_unique_)[PUSHMI_COUNTER] = nullptr,                  \
    std::enable_if_t<_pushmi_concept_unique_ ||                                \
        bool(PUSHMI_PP_CAT(PUSHMI_TEMPLATE_AUX_3_, __VA_ARGS__)), int> = 0>
#define PUSHMI_TEMPLATE_AUX_3_requires

namespace pushmi {
namespace concepts {
namespace detail {
template <class>
inline constexpr bool requires_() {
  return true;
}
template<typename T>
bool gcc_bugs_bugs_bugs(T);
}
}
template <class T>
PUSHMI_INLINE_VAR constexpr bool typename_ = true;
template <class T>
constexpr bool implicitly_convertible_to(T) {
  return true;
}
template <bool B>
PUSHMI_INLINE_VAR constexpr std::enable_if_t<B, bool> requires_ = true;
}

PUSHMI_PP_IGNORE_CXX2A_COMPAT_END
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <functional>
//#include <type_traits>

//#include <meta/meta.hpp>

//#include "detail/concept_def.h"

namespace pushmi {
namespace detail {
  template <bool...>
  struct bools;
}

#if __cpp_fold_expressions >= 201603
template <bool...Bs>
PUSHMI_INLINE_VAR constexpr bool all_true_v = (Bs &&...);
#else
template <bool...Bs>
PUSHMI_INLINE_VAR constexpr bool all_true_v =
  std::is_same<detail::bools<Bs..., true>, detail::bools<true, Bs...>>::value;
#endif

#if __cpp_fold_expressions >= 201603
template <bool...Bs>
PUSHMI_INLINE_VAR constexpr bool any_true_v = (Bs ||...);
#else
template <bool...Bs>
PUSHMI_INLINE_VAR constexpr bool any_true_v =
  !std::is_same<detail::bools<Bs..., false>, detail::bools<false, Bs...>>::value;
#endif

#if __cpp_fold_expressions >= 201603
template <int...Is>
PUSHMI_INLINE_VAR constexpr int sum_v = (Is +...);
#else
template <std::size_t N>
constexpr int sum_impl(int const (&rgi)[N], int i = 0, int state = 0) noexcept {
  return i == N ? state : sum_impl(rgi, i+1, state + rgi[i]);
}
template <int... Is>
constexpr int sum_impl() noexcept {
  using RGI = int[sizeof...(Is)];
  return sum_impl(RGI{Is...});
}
template <int...Is>
PUSHMI_INLINE_VAR constexpr int sum_v = sum_impl<Is...>();
#endif

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

PUSHMI_CONCEPT_DEF(
  template(class T, template<class> class C)
  (concept Valid)(T, C),
    requires () (
      typename_<C<T>>
    )
);

PUSHMI_CONCEPT_DEF(
  template (class T, template<class...> class Trait, class... Args)
  (concept Satisfies)(T, Trait, Args...),
    bool(Trait<T>::type::value)
);

PUSHMI_CONCEPT_DEF(
  template (class T, class U)
  concept Same,
    __is_same_as(T, U) && __is_same_as(U, T)
);

PUSHMI_CONCEPT_DEF(
  template (bool...Bs)
  (concept And)(Bs...),
    all_true_v<Bs...>
);

PUSHMI_CONCEPT_DEF(
  template (bool...Bs)
  (concept Or)(Bs...),
    any_true_v<Bs...>
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept Object,
    requires (T* p) (
      *p,
      implicitly_convertible_to<const volatile void*>(p)
    )
);

PUSHMI_CONCEPT_DEF(
  template (class T, class... Args)
  (concept Constructible)(T, Args...),
    __is_constructible(T, Args...)
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept MoveConstructible,
    Constructible<T, T>
);

PUSHMI_CONCEPT_DEF(
  template (class From, class To)
  concept ConvertibleTo,
    requires (From (&f)()) (
      static_cast<To>(f())
    ) && std::is_convertible<From, To>::value
);

PUSHMI_CONCEPT_DEF(
  template (class A, class B)
  concept Derived,
    __is_base_of(B, A)
);

PUSHMI_CONCEPT_DEF(
  template (class A)
  concept Decayed,
    Same<A, std::decay_t<A>>
);

PUSHMI_CONCEPT_DEF(
  template (class T, class U)
  concept Assignable,
    requires(T t, U&& u) (
      t = (U &&) u,
      requires_<Same<decltype(t = (U &&) u), T>>
    ) && Same<T, T&>
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept EqualityComparable,
    requires(remove_cvref_t<T> const & t) (
      implicitly_convertible_to<bool>( t == t ),
      implicitly_convertible_to<bool>( t != t )
    )
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept SemiMovable,
    Object<T> && Constructible<T, T> && ConvertibleTo<T, T>
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept Movable,
    SemiMovable<T> && Assignable<T&, T>
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept Copyable,
    Movable<T> &&
    Assignable<T&, const T&> &&
    ConvertibleTo<const T&, T>
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept Semiregular,
    Copyable<T> && Constructible<T>
);

PUSHMI_CONCEPT_DEF(
  template (class T)
  concept Regular,
    Semiregular<T> && EqualityComparable<T>
);

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
template <class F, class...As>
  requires Satisfies<F, std::is_member_pointer> &&
    requires (F f, As&&...as) { std::mem_fn(f)((As&&) as...); }
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
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <functional>

//#include <meta/meta.hpp>

//#include "../traits.h"
//#include "../forwards.h"

namespace pushmi {

namespace detail {

struct placeholder;

template <class T, class>
struct replace {
  using type = T;
};
template <std::size_t I, class Args>
struct replace<placeholder[I], Args>
  : meta::lazy::let<
      meta::defer<std::decay_t, meta::lazy::at<Args, meta::size_t<I-1>>>> {
};
template <std::size_t I, class Args>
struct replace<placeholder(&&)[I], Args>
  : meta::lazy::at<Args, meta::size_t<I-1>> {
};

template <class Requirement, class Args>
struct substitute {
};
template <template <class...> class R, class... Ts, class Args>
  requires requires { typename R<meta::_t<replace<Ts, Args>>...>; }
struct substitute<R<Ts...>, Args> {
  using type = R<meta::_t<replace<Ts, Args>>...>;
};

template <class Fn, class...Requirements>
struct constrained_fn : Fn {
  constrained_fn() = default;
  constrained_fn(Fn fn) : Fn(std::move(fn)) {}

  template <class... Ts>
    requires Invocable<Fn&, Ts...> &&
      And<Invocable<meta::_t<substitute<Requirements, meta::list<Ts...>>>>...>
  decltype(auto) operator()(Ts&&... ts) noexcept(noexcept(std::declval<Fn&>()((Ts&&) ts...))) {
    return static_cast<Fn&>(*this)((Ts&&) ts...);
  }
  template <class... Ts>
    requires Invocable<const Fn&, Ts...> &&
      And<Invocable<meta::_t<substitute<Requirements, meta::list<Ts...>>>>...>
  decltype(auto) operator()(Ts&&... ts) const noexcept(noexcept(std::declval<const Fn&>()((Ts&&) ts...))) {
    return static_cast<const Fn&>(*this)((Ts&&) ts...);
  }
};

template <class...Requirements>
struct constrain_fn {
  template <class Fn>
  constexpr auto operator()(Fn fn) const {
    return constrained_fn<Fn, Requirements...>{std::move(fn)};
  }
};

} // namespace detail

using _1 = detail::placeholder[1];
using _2 = detail::placeholder[2];
using _3 = detail::placeholder[3];

template <class...Requirements>
PUSHMI_INLINE_VAR constexpr const detail::constrain_fn<Requirements...> constrain {};

} // namespace pushmi
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.


// Usage:
//
// PUSHMI_IF_CONSTEXPR((condition)(
//     stmt1;
//     stmt2;
// ) else (
//     stmt3;
//     stmt4;
// ))
//
// If the statements could potentially be ill-formed, you can give some
// part of the expression a dependent type by wrapping it in `id`. For

#define PUSHMI_COMMA ,

#define PUSHMI_EVAL(F, ...) F(__VA_ARGS__)

#define PUSHMI_STRIP(...) __VA_ARGS__


namespace pushmi {
namespace detail {
struct id_fn {
  constexpr explicit operator bool() const noexcept {
      return false;
  }
  template <class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return (T&&) t;
  }
};
}
}

#if __cpp_if_constexpr >= 201606

#define PUSHMI_IF_CONSTEXPR(LIST)\
  if constexpr (::pushmi::detail::id_fn id = {}) {} \
  else if constexpr PUSHMI_EVAL(PUSHMI_IF_CONSTEXPR_ELSE_, PUSHMI_IF_CONSTEXPR_IF_ LIST)

#define PUSHMI_IF_CONSTEXPR_RETURN(LIST)\
  PUSHMI_IF_CONSTEXPR(LIST)\
  /**/

#define PUSHMI_IF_CONSTEXPR_IF_(...) \
  (__VA_ARGS__) PUSHMI_COMMA PUSHMI_IF_CONSTEXPR_THEN_

#define PUSHMI_IF_CONSTEXPR_THEN_(...) \
  ({__VA_ARGS__}) PUSHMI_COMMA

#define PUSHMI_IF_CONSTEXPR_ELSE_(A, B, C) \
  A PUSHMI_STRIP B PUSHMI_IF_CONSTEXPR_ ## C

#define PUSHMI_IF_CONSTEXPR_else(...) \
  else {__VA_ARGS__}

#else

#define PUSHMI_IF_CONSTEXPR(LIST)\
  PUSHMI_EVAL(PUSHMI_IF_CONSTEXPR_ELSE_, PUSHMI_IF_CONSTEXPR_IF_ LIST)\
  /**/

#define PUSHMI_IF_CONSTEXPR_RETURN(LIST)\
  return PUSHMI_EVAL(PUSHMI_IF_CONSTEXPR_ELSE_, PUSHMI_IF_CONSTEXPR_IF_ LIST)\
  /**/

#define PUSHMI_IF_CONSTEXPR_IF_(...) \
  (::pushmi::detail::select<bool(__VA_ARGS__)>() ->* PUSHMI_IF_CONSTEXPR_THEN_ \
  /**/

#define PUSHMI_IF_CONSTEXPR_THEN_(...) \
  ([&](auto id)->decltype(auto){__VA_ARGS__})) PUSHMI_COMMA \
  /**/

#define PUSHMI_IF_CONSTEXPR_ELSE_(A, B) \
  A ->* PUSHMI_IF_CONSTEXPR_ ## B \
  /**/

#define PUSHMI_IF_CONSTEXPR_else(...) ([&](auto id)->decltype(auto){__VA_ARGS__});

namespace pushmi {
namespace detail {

template <bool>
struct select {
    template <class R, class = std::enable_if_t<!std::is_void<R>::value>>
    struct eat_return {
        R value_;
        template <class T>
        constexpr R operator->*(T&&) {
          return static_cast<R&&>(value_);
        }
    };
    struct eat {
        template <class T>
        constexpr void operator->*(T&&) {}
    };
    template <class T>
    constexpr auto operator->*(T&& t) -> eat_return<decltype(t(::pushmi::detail::id_fn{}))> {
        return {t(::pushmi::detail::id_fn{})};
    }
    template <class T>
    constexpr auto operator->*(T&& t) const -> eat {
        return t(::pushmi::detail::id_fn{}), void(), eat{};
    }
};

template <>
struct select<false> {
    struct eat {
        template <class T>
        constexpr auto operator->*(T&& t) -> decltype(auto) {
            return t(::pushmi::detail::id_fn{});
        }
    };
    template <class T>
    constexpr eat operator->*(T&& t) {
        return {};
    }
};
}
}
#endif
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#if __cpp_lib_optional >= 201606
//#include <optional>
#endif
//#include <type_traits>

namespace pushmi {
namespace detail {
#if __cpp_lib_optional >= 201606
template <class T>
struct opt : private std::optional<T> {
   opt() = default;
   opt& operator=(T&& t) {
     this->std::optional<T>::operator=(std::move(t));
     return *this;
   }
   using std::optional<T>::operator*;
   using std::optional<T>::operator bool;
};
#else
template <class T>
struct opt {
private:
  bool empty_ = true;
  std::aligned_union_t<0, T> data_;
  T* ptr() {
    return static_cast<T*>((void*)&data_);
  }
  const T* ptr() const {
    return static_cast<const T*>((const void*)&data_);
  }
  void reset() {
    if (!empty_) {
      ptr()->~T();
      empty_ = true;
    }
  }
public:
  opt() = default;
  opt(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value) {
    ::new(ptr()) T(std::move(t));
    empty_ = false;
  }
  opt(const T& t) {
    ::new(ptr()) T(t);
    empty_ = false;
  }
  opt(opt&& that) noexcept(std::is_nothrow_move_constructible<T>::value) {
    if (that) {
      ::new(ptr()) T(std::move(*that));
      empty_ = false;
      that.reset();
    }
  }
  opt(const opt& that) {
    if (that) {
      ::new(ptr()) T(*that);
      empty_ = false;
    }
  }
  ~opt() { reset(); }
  opt& operator=(opt&& that)
    noexcept(std::is_nothrow_move_constructible<T>::value &&
             std::is_nothrow_move_assignable<T>::value) {
    if (*this && that) {
      **this = std::move(*that);
      that.reset();
    } else if (*this) {
      reset();
    } else if (that) {
      ::new(ptr()) T(std::move(*that));
      empty_ = false;
    }
    return *this;
  }
  opt& operator=(const opt& that) {
    if (*this && that) {
      **this = *that;
    } else if (*this) {
      reset();
    } else if (that) {
      ::new(ptr()) T(*that);
      empty_ = false;
    }
    return *this;
  }
  opt& operator=(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                 std::is_nothrow_move_assignable<T>::value) {
    if (*this)
      **this = std::move(t);
    else {
      ::new(ptr()) T(std::move(t));
      empty_ = false;
    }
    return *this;
  }
  opt& operator=(const T& t) {
    if (*this)
      **this = t;
    else {
      ::new(ptr()) T(t);
      empty_ = false;
    }
    return *this;
  }
  explicit operator bool() const noexcept {
    return !empty_;
  }
  T& operator*() noexcept {
    return *ptr();
  }
  const T& operator*() const noexcept {
    return *ptr();
  }
};
#endif

} // namespace detail
} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <exception>
//#include <chrono>
//#include "traits.h"

namespace pushmi {

// property_set

template <class T>
struct property_traits;

template <class T>
struct property_set_traits;

template<class... PropertyN>
struct property_set;

// trait & tag types
template<class...TN>
struct is_silent;
template<class...TN>
struct is_none;
template<class...TN>
struct is_single;
template<class...TN>
struct is_many;

template<class...TN>
struct is_flow;

template<class...TN>
struct is_receiver;

template<class...TN>
struct is_sender;

template<class...TN>
struct is_time;
template<class...TN>
struct is_constrained;

// implementation types

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class none;

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class deferred;

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class single;

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class single_deferred;

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class time_single_deferred;

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class flow_single;

template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... TN>
class flow_single_deferred;

template<
  class E = std::exception_ptr,
  class TP = std::chrono::system_clock::time_point,
  int i = 0>
struct any_time_executor_ref;

namespace operators {}
namespace extension_operators {}
namespace aliases {
    namespace v = ::pushmi;
    namespace op = ::pushmi::operators;
    namespace ep = ::pushmi::extension_operators;
}

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <future>
//#include <functional>

//#include "traits.h"

namespace pushmi {
namespace __adl {
template <class S>
requires requires(S& s) {
  s.done();
}
void set_done(S& s) noexcept(noexcept(s.done())) {
  s.done();
}
template <class S, class E>
requires requires(S& s, E e) {
  s.error(std::move(e));
}
void set_error(S& s, E e) noexcept(noexcept(s.error(std::move(e)))) {
  s.error(std::move(e));
}
template <class S, class V>
requires requires(S& s, V&& v) {
  s.value((V&&) v);
}
void set_value(S& s, V&& v) noexcept(noexcept(s.value((V&&) v))) {
  s.value((V&&) v);
}

template <class S>
requires requires(S& s) {
  s.stopping();
}
void set_stopping(S& s) noexcept(noexcept(s.stopping())) {
  s.stopping();
}
template <class S, class Up>
requires requires(S& s, Up& up) {
  s.starting(up);
}
void set_starting(S& s, Up& up) noexcept(noexcept(s.starting(up))) {
  s.starting(up);
}

template <class SD, class Out>
requires requires(SD& sd, Out out) {
  sd.submit(std::move(out));
}
void submit(SD& sd, Out out) noexcept(noexcept(sd.submit(std::move(out)))) {
  sd.submit(std::move(out));
}

template <class SD>
requires requires(SD& sd) {
  sd.now();
}
auto now(SD& sd) noexcept(noexcept(sd.now())) {
  return sd.now();
}

template <class SD, class TP, class Out>
requires requires(SD& sd, TP tp, Out out) {
  { sd.now() } -> TP;
  sd.submit(std::move(tp), std::move(out));
}
void submit(SD& sd, TP tp, Out out)
  noexcept(noexcept(sd.submit(std::move(tp), std::move(out)))) {
  sd.submit(std::move(tp), std::move(out));
}

template <class T>
void set_done(std::promise<T>& p) noexcept(
    noexcept(p.set_exception(std::make_exception_ptr(0)))) {
  p.set_exception(std::make_exception_ptr(
      std::logic_error("std::promise does not support done.")));
}
inline void set_done(std::promise<void>& p) noexcept(noexcept(p.set_value())) {
  p.set_value();
}
template <class T>
void set_error(std::promise<T>& s, std::exception_ptr e) noexcept {
  s.set_exception(std::move(e));
}
template <class T, class E>
void set_error(std::promise<T>& s, E e) noexcept {
  s.set_exception(std::make_exception_ptr(std::move(e)));
}
template <class T>
requires !Same<T, void>
void set_value(std::promise<T>& s, T t) {
  s.set_value(std::move(t));
}

template <class S>
requires requires (S& s) { set_done(s); }
void set_done(std::reference_wrapper<S> s) noexcept(
  noexcept(set_done(s.get()))) {
  set_done(s.get());
}
template <class S, class E>
requires requires (S& s, E e) { set_error(s, std::move(e)); }
void set_error(std::reference_wrapper<S> s, E e) noexcept {
  set_error(s.get(), std::move(e));
}
template <class S, class V>
requires requires (S& s, V&& v) { set_value(s, (V&&) v); }
void set_value(std::reference_wrapper<S> s, V&& v) noexcept(
  noexcept(set_value(s.get(), (V&&) v))) {
  set_value(s.get(), (V&&) v);
}
template <class S>
requires requires(S& s) { set_stopping(s); }
void set_stopping(std::reference_wrapper<S> s) noexcept(
  noexcept(set_stopping(s.get()))) {
  set_stopping(s.get());
}
template <class S, class Up>
requires requires(S& s, Up& up) { set_starting(s, up); }
void set_starting(std::reference_wrapper<S> s, Up& up) noexcept(
  noexcept(set_starting(s.get(), up))) {
  set_starting(s.get(), up);
}
template <class SD, class Out>
requires requires(SD& sd, Out out) { submit(sd, std::move(out)); }
void submit(std::reference_wrapper<SD> sd, Out out) noexcept(
  noexcept(submit(sd.get(), std::move(out)))) {
  submit(sd.get(), std::move(out));
}
template <class SD>
requires requires(SD& sd) { now(sd); }
auto now(std::reference_wrapper<SD> sd) noexcept(noexcept(now(sd.get()))) {
  return now(sd.get());
}
template <class SD, class TP, class Out>
requires requires(SD& sd, TP tp, Out out) {
  submit(sd, std::move(tp), std::move(out));
}
void submit(std::reference_wrapper<SD> sd, TP tp, Out out)
  noexcept(noexcept(submit(sd.get(), std::move(tp), std::move(out)))) {
  submit(sd.get(), std::move(tp), std::move(out));
}


struct set_done_fn {
  template <class S>
  requires requires(S& s) {
    set_done(s);
    set_error(s, std::current_exception());
  }
  void operator()(S&& s) const noexcept(noexcept(set_done(s))) {
    try {
      set_done(s);
    } catch (...) {
      set_error(s, std::current_exception());
    }
  }
};
struct set_error_fn {
  template <class S, class E>
  requires requires(S& s, E e) {
    { set_error(s, std::move(e)) } noexcept;
  }
  void operator()(S&& s, E e) const
      noexcept(noexcept(set_error(s, std::move(e)))) {
    set_error(s, std::move(e));
  }
};
struct set_value_fn {
  template <class S, class V>
  requires requires(S& s, V&& v) {
    set_value(s, (V&&) v);
    set_error(s, std::current_exception());
  }
  void operator()(S&& s, V&& v) const
      noexcept(noexcept(set_value(s, (V&&) v))) {
    try {
      set_value(s, (V&&) v);
    } catch (...) {
      set_error(s, std::current_exception());
    }
  }
};

struct set_stopping_fn {
  template <class S>
  requires requires(S& s) {
    set_stopping(s);
  }
  void operator()(S&& s) const noexcept(noexcept(set_stopping(s))) {
    set_stopping(s);
  }
};
struct set_starting_fn {
  template <class S, class Up>
  requires requires(S& s, Up& up) {
    set_starting(s, up);
    set_error(s, std::current_exception());
  }
  void operator()(S&& s, Up& up) const
      noexcept(noexcept(set_starting(s, up))) {
    try {
      set_starting(s, up);
    } catch (...) {
      set_error(s, std::current_exception());
    }
  }
};

struct do_submit_fn {
  template <class SD, class Out>
  requires requires(SD& s, Out out) {
    submit(s, std::move(out));
  }
  void operator()(SD&& s, Out out) const
      noexcept(noexcept(submit(s, std::move(out)))) {
    submit(s, std::move(out));
  }

  template <class SD, class TP, class Out>
  requires requires(SD& s, TP tp, Out out) {
    submit(s, std::move(tp), std::move(out));
  }
  void operator()(SD&& s, TP tp, Out out) const
      noexcept(noexcept(submit(s, std::move(tp), std::move(out)))) {
    submit(s, std::move(tp), std::move(out));
  }
};

struct get_now_fn {
  template <class SD>
  requires requires(SD& sd) {
    now(sd);
  }
  auto operator()(SD&& sd) const noexcept(noexcept(now(sd))) {
    return now(sd);
  }
};

} // namespace __adl

PUSHMI_INLINE_VAR constexpr __adl::set_done_fn set_done{};
PUSHMI_INLINE_VAR constexpr __adl::set_error_fn set_error{};
PUSHMI_INLINE_VAR constexpr __adl::set_value_fn set_value{};
PUSHMI_INLINE_VAR constexpr __adl::set_stopping_fn set_stopping{};
PUSHMI_INLINE_VAR constexpr __adl::set_starting_fn set_starting{};
PUSHMI_INLINE_VAR constexpr __adl::do_submit_fn submit{};
PUSHMI_INLINE_VAR constexpr __adl::get_now_fn now{};
PUSHMI_INLINE_VAR constexpr __adl::get_now_fn top{};

template <class T>
struct property_set_traits<std::promise<T>> {
  using properties = property_set<is_receiver<>, is_single<>>;
};
template <>
struct property_set_traits<std::promise<void>> {
  using properties = property_set<is_receiver<>, is_none<>>;
};

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "traits.h"

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
// clang-format off
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "forwards.h"
//#include "extension_points.h"
//#include "properties.h"

namespace pushmi {

// traits & tags

// cardinality affects both sender and receiver

struct cardinality_category {};

// flow affects both sender and receiver

struct flow_category {};

// sender and receiver are mutually exclusive

struct receiver_category {};

struct sender_category {};

// time and constrained are mutually exclusive refinements of sender (time is a special case of constrained and may be folded in later)


// Silent trait and tag
template<class... TN>
struct is_silent;
// Tag
template<>
struct is_silent<> { using property_category = cardinality_category; };
// Trait
template<class PS>
struct is_silent<PS> : property_query<PS, is_silent<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_silent_v = is_silent<PS>::value;
template <class PS>
concept bool Silent = is_silent_v<PS>;

// None trait and tag
template<class... TN>
struct is_none;
// Tag
template<>
struct is_none<> : is_silent<> {};
// Trait
template<class PS>
struct is_none<PS> : property_query<PS, is_none<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_none_v = is_none<PS>::value;
template <class PS>
concept bool None = is_none_v<PS>;

// Single trait and tag
template<class... TN>
struct is_single;
// Tag
template<>
struct is_single<> : is_none<> {};
// Trait
template<class PS>
struct is_single<PS> : property_query<PS, is_single<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_single_v = is_single<PS>::value;
template <class PS>
concept bool Single = is_single_v<PS>;

// Many trait and tag
template<class... TN>
struct is_many;
// Tag
template<>
struct is_many<> : is_none<> {}; // many::value() does not terminate, so it is not a refinement of single
// Trait
template<class PS>
struct is_many<PS> : property_query<PS, is_many<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_many_v = is_many<PS>::value;
template <class PS>
concept bool Many = is_many_v<PS>;


// Flow trait and tag
template<class... TN>
struct is_flow;
// Tag
template<>
struct is_flow<> { using property_category = flow_category; };
// Trait
template<class PS>
struct is_flow<PS> : property_query<PS, is_flow<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_flow_v = is_flow<PS>::value;
template <class PS>
concept bool Flow = is_flow_v<PS>;


// Receiver trait and tag
template<class... TN>
struct is_receiver;
// Tag
template<>
struct is_receiver<> { using property_category = receiver_category; };
// Trait
template<class PS>
struct is_receiver<PS> : property_query<PS, is_receiver<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_receiver_v = is_receiver<PS>::value;
// template <class PS>
// concept bool Receiver = is_receiver_v<PS>;


// Sender trait and tag
template<class... TN>
struct is_sender;
// Tag
template<>
struct is_sender<> { using property_category = sender_category; };
// Trait
template<class PS>
struct is_sender<PS> : property_query<PS, is_sender<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_sender_v = is_sender<PS>::value;
// template <class PS>
// concept bool Sender = is_sender_v<PS>;

// Time trait and tag
template<class... TN>
struct is_time;
// Tag
template<>
struct is_time<> : is_sender<> {};
// Trait
template<class PS>
struct is_time<PS> : property_query<PS, is_time<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_time_v = is_time<PS>::value;
template <class PS>
concept bool Time = is_time_v<PS>;

// Constrained trait and tag
template<class... TN>
struct is_constrained;
// Tag
template<>
struct is_constrained<> : is_sender<> {};
// Trait
template<class PS>
struct is_constrained<PS> : property_query<PS, is_constrained<>> {};
template<class PS>
PUSHMI_INLINE_VAR constexpr bool is_constrained_v = is_constrained<PS>::value;
template <class PS>
concept bool Constrained = is_constrained_v<PS>;


template <class S, class... PropertyN>
concept bool Receiver = SemiMovable<S> && 
  property_query_v<S, PropertyN...> &&
  is_receiver_v<S> &&
  requires (S& s) {
    ::pushmi::set_done(s);
  };

template <class N, class E = std::exception_ptr>
concept bool NoneReceiver = Receiver<N> &&
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
concept bool ManyReceiver = NoneReceiver<S, E> &&
  SemiMovable<T> &&
  SemiMovable<E> &&
  Many<S>;




// silent does not really make sense, but cannot test for
// None without the error type, use is_none<> to strengthen 
// requirements
template <class D, class... PropertyN>
concept bool Sender = SemiMovable<D> &&
  None<D> && 
  property_query_v<D, PropertyN...> &&
  is_sender_v<D>;

template <class D, class S, class... PropertyN>
concept bool SenderTo = Sender<D> &&
  Receiver<S> &&
  property_query_v<D, PropertyN...> &&
  requires(D& d, S&& s) {
    ::pushmi::submit(d, (S &&) s);
  };


// add concepts to support cancellation
//

template <class S, class... PropertyN>
concept bool FlowReceiver = Receiver<S> &&
  property_query_v<S, PropertyN...> &&
  Flow<S> &&
  requires(S& s) {
    ::pushmi::set_stopping(s);
  };

template <
  class N, 
  class Up, 
  class PE = std::exception_ptr,
  class E = PE>
concept bool FlowNoneReceiver = FlowReceiver<N> && 
  Receiver<Up> &&
  SemiMovable<PE> &&
  SemiMovable<E> &&
  NoneReceiver<Up, PE> && 
  NoneReceiver<N, E> &&
  requires(N& n, Up& up) {
    ::pushmi::set_starting(n, up);
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

template <class S, class... PropertyN>
concept bool FlowSender = Sender<S> &&
  property_query_v<S, PropertyN...> &&
  Flow<S>;

template <class D, class S, class... PropertyN>
concept bool FlowSenderTo = FlowSender<D> &&
  property_query_v<D, PropertyN...> &&
  FlowReceiver<S>;

// add concepts for constraints
//

template <class D, class... PropertyN>
concept bool TimeSender = Sender<D> && 
  property_query_v<D, PropertyN...> &&
  Time<D> && 
  None<D> &&
  requires(D& d) {
    ::pushmi::now(d);
    requires Regular<decltype(::pushmi::now(d))>;
  };

template <class D, class S, class... PropertyN>
concept bool TimeSenderTo = TimeSender<D> && 
  property_query_v<D, PropertyN...> &&
  Receiver<S> &&
  requires(D& d, S&& s) {
    ::pushmi::submit(d, ::pushmi::now(d), (S &&) s);
  };

template <class D>
  requires TimeSender<D>
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
concept bool ConstrainedSender = Sender<D> &&
  Constrained<D> &&
  None<D> && 
  requires(D& d) {
    ::pushmi::top(d);
    requires Regular<decltype(::pushmi::top(d))>;
  };

template <class D, class S>
concept bool ConstrainedSenderTo = ConstrainedSender<D> && 
  Receiver<S> &&
  requires(D& d, S&& s) {
    ::pushmi::submit(d, ::pushmi::top(d), (S &&) s);
  };

template <class D>
  requires ConstrainedSender<D>
using constraint_t = decltype(::pushmi::top(std::declval<D&>()));




// Mock concepts for use with the constrain() function template for
// constraining generic lambdas.
namespace mock {

template <class Out, class Tag = is_silent<>>
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

template <class D, class Tag = is_silent<>>
struct Sender {
  void operator()() requires pushmi::Sender<D, Tag> {}
};
template <class D, class S, class Tag = is_silent<>>
struct SenderTo {
  void operator()() requires pushmi::SenderTo<D, S, Tag> {}
};

template <class D, class Tag = is_silent<>>
struct TimeSender {
  void operator()() requires pushmi::TimeSender<D, Tag> {}
};
template <class D, class S, class Tag = is_silent<>>
struct TimeSenderTo {
  void operator()() requires pushmi::TimeSenderTo<D, S, Tag> {}
};

} // namespace mock

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <chrono>
//#include <cstdint>
//#include <cstdio>
//#include <exception>
//#include <functional>
//#include <utility>

//#include "concepts.h"

namespace pushmi {

template<class T>
struct construct {
  template<class... AN>
    requires Constructible<T, AN...>
  auto operator()(AN&&... an) const {
    return T{std::forward<AN>(an)...};
  }
};


template<template <class...> class T>
struct construct_deduced;

template<>
struct construct_deduced<none>;

template<>
struct construct_deduced<single>;

template <template <class...> class T, class... AN>
using deduced_type_t = pushmi::invoke_result_t<construct_deduced<T>, AN...>;

struct ignoreVF {
  template <class V>
  void operator()(V&&) {}
};

struct abortEF {
  template <class E>
  void operator()(E) noexcept {
    std::abort();
  }
};

struct ignoreDF {
  void operator()() {}
};

struct ignoreStpF {
  void operator()() {}
};

struct ignoreStrtF {
  template <class Up>
  void operator()(Up&) {}
};


struct ignoreSF {
  template <class Out>
  void operator()(Out) {}
  template <class TP, class Out>
  void operator()(TP, Out) {}
};

struct systemNowF {
  auto operator()() { return std::chrono::system_clock::now(); }
};

struct passDVF {
  template <class V, Receiver Data>
  requires requires(Data& out, V&& v) {
    ::pushmi::set_value(out, (V&&) v);
  }
  void operator()(Data& out, V&& v) const {
    ::pushmi::set_value(out, (V&&) v);
  }
};

struct passDEF {
  template <class E, NoneReceiver<E> Data>
  void operator()(Data& out, E e) const noexcept {
    ::pushmi::set_error(out, e);
  }
};

struct passDDF {
  template <Receiver Data>
  void operator()(Data& out) const {
    ::pushmi::set_done(out);
  }
};

struct passDStpF {
  template <Receiver Data>
  void operator()(Data& out) const {
    ::pushmi::set_stopping(out);
  }
};

struct passDStrtF {
  template <class Up, Receiver Data>
  requires requires(Data& out, Up& up) {
    ::pushmi::set_starting(out, up);
  }
  void operator()(Data& out, Up& up) const {
    ::pushmi::set_starting(out, up);
  }
};


struct passDSF {
  template <class Data, class Out>
  void operator()(Data& in, Out out) {
    ::pushmi::submit(in, std::move(out));
  }
  template <class Data, class TP, class Out>
  void operator()(Data& in, TP at, Out out) {
    ::pushmi::submit(in, std::move(at), std::move(out));
  }
};

struct passDNF {
  template <TimeSender Data>
  auto operator()(Data& in) const noexcept {
    return ::pushmi::now(in);
  }
};

// inspired by Ovrld - shown in a presentation by Nicolai Josuttis
#if __cpp_variadic_using >= 201611
template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... Fns>
  requires sizeof...(Fns) > 0
struct overload_fn : Fns... {
  constexpr overload_fn() = default;
  constexpr explicit overload_fn(Fns... fns) requires sizeof...(Fns) == 1
      : Fns(std::move(fns))... {}
  constexpr overload_fn(Fns... fns) requires sizeof...(Fns) > 1
      : Fns(std::move(fns))... {}
  using Fns::operator()...;
};
#else
template <PUSHMI_TYPE_CONSTRAINT(SemiMovable)... Fns>
  requires sizeof...(Fns) > 0
struct overload_fn;
template <class Fn>
struct overload_fn<Fn> : Fn {
  constexpr overload_fn() = default;
  constexpr explicit overload_fn(Fn fn)
      : Fn(std::move(fn)) {}
  using Fn::operator();
};
template <class Fn, class... Fns>
struct overload_fn<Fn, Fns...> : Fn, overload_fn<Fns...> {
  constexpr overload_fn() = default;
  constexpr overload_fn(Fn fn, Fns... fns)
      : Fn(std::move(fn)), overload_fn<Fns...>{std::move(fns)...} {}
  using Fn::operator();
  using overload_fn<Fns...>::operator();
};
#endif

template <class... Fns>
auto overload(Fns... fns) -> overload_fn<Fns...> {
  return overload_fn<Fns...>{std::move(fns)...};
}

template <class... Fns>
struct on_value_fn : overload_fn<Fns...> {
  constexpr on_value_fn() = default;
  using overload_fn<Fns...>::overload_fn;
};

template <class... Fns>
auto on_value(Fns... fns) -> on_value_fn<Fns...> {
  return on_value_fn<Fns...>{std::move(fns)...};
}

template <class... Fns>
struct on_error_fn : overload_fn<Fns...> {
  constexpr on_error_fn() = default;
  using overload_fn<Fns...>::overload_fn;
};

template <class... Fns>
auto on_error(Fns... fns) -> on_error_fn<Fns...> {
  return on_error_fn<Fns...>{std::move(fns)...};
}

template <class Fn>
struct on_done_fn : Fn {
  constexpr on_done_fn() = default;
  constexpr explicit on_done_fn(Fn fn) : Fn(std::move(fn)) {}
  using Fn::operator();
};

template <class Fn>
auto on_done(Fn fn) -> on_done_fn<Fn> {
  return on_done_fn<Fn>{std::move(fn)};
}

template <class Fn>
struct on_stopping_fn : Fn {
  constexpr on_stopping_fn() = default;
  constexpr explicit on_stopping_fn(Fn fn) : Fn(std::move(fn)) {}
  using Fn::operator();
};

template <class Fn>
auto on_stopping(Fn fn) -> on_stopping_fn<Fn> {
  return on_stopping_fn<Fn>{std::move(fn)};
}

template <class... Fns>
struct on_starting_fn : overload_fn<Fns...> {
  constexpr on_starting_fn() = default;
  using overload_fn<Fns...>::overload_fn;
};

template <class... Fns>
auto on_starting(Fns... fns) -> on_starting_fn<Fns...> {
  return on_starting_fn<Fns...>{std::move(fns)...};
}

template <class... Fns>
struct on_submit_fn : overload_fn<Fns...> {
  constexpr on_submit_fn() = default;
  using overload_fn<Fns...>::overload_fn;
};

template <class... Fns>
auto on_submit(Fns... fns) -> on_submit_fn<Fns...> {
  return on_submit_fn<Fns...>{std::move(fns)...};
}

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "traits.h"

template <class In, pushmi::Invocable<In> Op>
decltype(auto) operator|(In&& in, Op op) {
  return op((In&&) in);
}

namespace pushmi {

PUSHMI_INLINE_VAR constexpr struct pipe_fn {
#if __cpp_fold_expressions >= 201603
  template<class T, class... FN>
  auto operator()(T t, FN... fn) const -> decltype((t | ... | fn)) {
    return (t | ... | fn);
  }
#else
  template<class T, class F>
  auto operator()(T t, F f) const -> decltype(t | f) {
    return t | f;
  }
  template<class T, class F, class... FN, class This = pipe_fn>
  auto operator()(T t, F f, FN... fn) const -> decltype(This()((t | f), fn...)) {
    return This()((t | f), fn...);
  }
#endif
} const pipe {};

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "boosters.h"

namespace pushmi {

template <class E>
class none<E> {
  bool done_ = false;
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(std::promise<int>)]; // can hold a std::promise in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static void s_done(data&) {}
    static void s_error(data&, E) noexcept { std::terminate(); };
    void (*op_)(data&, data*) = s_op;
    void (*done_)(data&) = s_done;
    void (*error_)(data&, E) noexcept = s_error;
    static constexpr vtable const noop_ {};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, none>::value, U>;
public:
  using properties = property_set<is_receiver<>, is_none<>>;

  none() = default;
  none(none&& that) noexcept : none() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped>
    requires NoneReceiver<wrapped_t<Wrapped>, E>
  explicit none(Wrapped obj) : none() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static void done(data& src) {
        ::pushmi::set_done(*static_cast<Wrapped*>(src.pobj_));
      }
      static void error(data& src, E e) noexcept {
          ::pushmi::set_error(*static_cast<Wrapped*>(src.pobj_), std::move(e));
      }
    };
    static const vtable vtable_v{s::op, s::done, s::error};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtable_v;
  }
  template <class Wrapped>
    requires NoneReceiver<wrapped_t<Wrapped>, E> && insitu<Wrapped>()
  explicit none(Wrapped obj) noexcept : none() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          new (dst->buffer_)
              Wrapped(std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
        static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static void done(data& src) {
        ::pushmi::set_done(*static_cast<Wrapped*>((void*)src.buffer_));
      }
      static void error(data& src, E e) noexcept {::pushmi::set_error(
          *static_cast<Wrapped*>((void*)src.buffer_),
          std::move(e));
      }
    };
    static const vtable vtbl{s::op, s::done, s::error};
    new (data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~none() {
    vptr_->op_(data_, nullptr);
  }
  none& operator=(none&& that) noexcept {
    this->~none();
    new ((void*)this) none(std::move(that));
    return *this;
  }
  void error(E e) noexcept {
    if (done_) {return;}
    done_ = true;
    vptr_->error_(data_, std::move(e));
  }
  void done() {
    if (done_) {return;}
    done_ = true;
    vptr_->done_(data_);
  }
};

// Class static definitions:
template <class E>
constexpr typename none<E>::vtable const none<E>::vtable::noop_;

template <class EF, class DF>
  requires Invocable<DF&>
class none<EF, DF> {
  static_assert(!detail::is_v<EF, on_value_fn> && !detail::is_v<EF, single>);
  bool done_ = false;
  EF ef_{};
  DF df_{};

public:
  using properties = property_set<is_receiver<>, is_none<>>;

  none() = default;
  constexpr explicit none(EF ef)
      : none(std::move(ef), DF{}) {}
  constexpr explicit none(DF df)
      : none(EF{}, std::move(df)) {}
  constexpr none(EF ef, DF df)
      : done_(false), ef_(std::move(ef)), df_(std::move(df)) {}

  template <class E>
    requires Invocable<EF&, E>
  void error(E e) noexcept {
    static_assert(
        noexcept(ef_(std::move(e))),
        "error function must be noexcept");
    if (!done_) {
      done_ = true;
      ef_(std::move(e));
    }
  }
  void done() {
    if (!done_) {
      done_ = true;
      df_();
    }
  }
};

template <Receiver<is_none<>> Data, class DEF, class DDF>
  requires Invocable<DDF&, Data&>
class none<Data, DEF, DDF> {
  bool done_ = false;
  Data data_{};
  DEF ef_{};
  DDF df_{};
  static_assert(!detail::is_v<DEF, on_value_fn>);
  static_assert(!detail::is_v<Data, single>);
public:
  using properties = property_set<is_receiver<>, is_none<>>;

  constexpr explicit none(Data d) : none(std::move(d), DEF{}, DDF{}) {}
  constexpr none(Data d, DDF df)
      : done_(false), data_(std::move(d)), ef_(), df_(std::move(df)) {}
  constexpr none(Data d, DEF ef, DDF df = DDF{})
      : done_(false), data_(std::move(d)), ef_(std::move(ef)),
        df_(std::move(df)) {}
  template <class E>
    requires Invocable<DEF&, Data&, E>
  void error(E e) noexcept {
    static_assert(
        noexcept(ef_(data_, std::move(e))), "error function must be noexcept");
    if (!done_) {
      done_ = true;
      ef_(data_, std::move(e));
    }
  }
  void done() {
    if (!done_) {
      done_ = true;
      df_(data_);
    }
  }
};

template <>
class none<>
    : public none<abortEF, ignoreDF> {
};

////////////////////////////////////////////////////////////////////////////////
// make_flow_single
inline auto make_none() -> none<> {
  return {};
}
template <class EF>
auto make_none(EF ef) -> none<EF, ignoreDF> {
  return none<EF, ignoreDF>{std::move(ef)};
}
template <class DF>
  requires Invocable<DF&>
auto make_none(DF df) -> none<abortEF, DF> {
  return none<abortEF, DF>{std::move(df)};
}
template <class EF, class DF>
  requires Invocable<DF&>
auto make_none(EF ef, DF df) -> none<EF, DF> {
  return {std::move(ef), std::move(df)};
}
template <Receiver<is_none<>> Data>
  requires !Receiver<Data, is_single<>>
auto make_none(Data d) -> none<Data, passDEF, passDDF> {
  return none<Data, passDEF, passDDF>{std::move(d)};
}
template <Receiver<is_none<>> Data, class DEF>
  requires !Receiver<Data, is_single<>>
auto make_none(Data d, DEF ef) -> none<Data, DEF, passDDF> {
  return {std::move(d), std::move(ef)};
}
template <Receiver<is_none<>> Data, class DDF>
  requires Invocable<DDF&, Data&> && !Receiver<Data, is_single<>>
auto make_none(Data d, DDF df) -> none<Data, passDEF, DDF> {
  return {std::move(d), std::move(df)};
}
template <Receiver<is_none<>> Data, class DEF, class DDF>
  requires !Receiver<Data, is_single<>>
auto make_none(Data d, DEF ef, DDF df) -> none<Data, DEF, DDF> {
  return {std::move(d), std::move(ef), std::move(df)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
none() -> none<>;

template <class EF>
none(EF) -> none<EF, ignoreDF>;

template <class DF>
  requires Invocable<DF&>
none(DF) -> none<abortEF, DF>;

template <class EF, class DF>
  requires Invocable<DF&>
none(EF, DF) -> none<EF, DF>;

template <Receiver<is_none<>> Data>
  requires !Receiver<Data, is_single<>>
none(Data) -> none<Data, passDEF, passDDF>;

template <Receiver<is_none<>> Data, class DEF>
  requires !Receiver<Data, is_single<>>
none(Data, DEF) -> none<Data, DEF, passDDF>;

template <Receiver<is_none<>> Data, class DDF>
  requires Invocable<DDF&, Data&> && !Receiver<Data, is_single<>>
none(Data, DDF) -> none<Data, passDEF, DDF>;

template <Receiver<is_none<>> Data, class DEF, class DDF>
  requires !Receiver<Data, is_single<>>
none(Data, DEF, DDF) -> none<Data, DEF, DDF>;
#endif

template <class E = std::exception_ptr>
using any_none = none<E>;

template<>
struct construct_deduced<none> {
  template<class... AN>
  auto operator()(AN&&... an) const -> decltype(pushmi::make_none((AN&&) an...)) {
    return pushmi::make_none((AN&&) an...);
  }
};

// // this is ambiguous because NoneReceiver and SingleReceiver only constrain the done() method.
// // template <class E = std::exception_ptr, NoneReceiver<E> Wrapped>
// // auto erase_cast(Wrapped w) {
// //   return none<erase_cast_t, E>{std::move(w)};
// // }
// template <class E = std::exception_ptr, class... TN>
// auto erase_cast(none<TN...> w) {
//   return none<E>{std::move(w)};
// }
// template <class E = std::exception_ptr>
// auto erase_cast(std::promise<void> w) {
//   return none<E>{std::move(w)};
// }

template <SenderTo<std::promise<void>, is_none<>> Out>
std::future<void> future_from(Out out) {
  std::promise<void> p;
  auto result = p.get_future();
  submit(out, std::move(p));
  return result;
}

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "none.h"

namespace pushmi {
namespace detail {
struct erase_deferred_t {};
} // namespace detail

template <class E>
class deferred<detail::erase_deferred_t, E> {
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(std::promise<void>)]; // can hold a void promise in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static void s_submit(data&, any_none<E>) {}
    void (*op_)(data&, data*) = s_op;
    void (*submit_)(data&, any_none<E>) = s_submit;
    static constexpr vtable const noop_{};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, deferred>::value, U>;
 public:
  using properties = property_set<is_sender<>, is_none<>>;

  deferred() = default;
  deferred(deferred&& that) noexcept : deferred() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped>
    requires SenderTo<wrapped_t<Wrapped>, any_none<E>, is_none<>>
  explicit deferred(Wrapped obj) : deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static void submit(data& src, any_none<E> out) {
        ::pushmi::submit(*static_cast<Wrapped*>(src.pobj_), std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::submit};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  template <class Wrapped>
    requires SenderTo<wrapped_t<Wrapped>, any_none<E>, is_none<>> && insitu<Wrapped>()
  explicit deferred(Wrapped obj) noexcept : deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          new (dst->buffer_) Wrapped(
              std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
        static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static void submit(data& src, any_none<E> out) {
        ::pushmi::submit(
            *static_cast<Wrapped*>((void*)src.buffer_), std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::submit};
    new (data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~deferred() {
    vptr_->op_(data_, nullptr);
  }
  deferred& operator=(deferred&& that) noexcept {
    this->~deferred();
    new ((void*)this) deferred(std::move(that));
    return *this;
  }
  void submit(any_none<E> out) {
    vptr_->submit_(data_, std::move(out));
  }
};

// Class static definitions:
template <class E>
constexpr typename deferred<detail::erase_deferred_t, E>::vtable const
    deferred<detail::erase_deferred_t, E>::vtable::noop_;

template <class SF>
class deferred<SF> {
  SF sf_{};

 public:
  using properties = property_set<is_sender<>, is_none<>>;

  constexpr deferred() = default;
  constexpr explicit deferred(SF sf) : sf_(std::move(sf)) {}
  template <Receiver<is_none<>> Out>
    requires Invocable<SF&, Out>
  void submit(Out out) {
    sf_(std::move(out));
  }
};

template <Sender<is_none<>> Data, class DSF>
class deferred<Data, DSF> {
  Data data_{};
  DSF sf_{};

 public:
  using properties = property_set<is_sender<>, is_none<>>;

  constexpr deferred() = default;
  constexpr explicit deferred(Data data)
      : data_(std::move(data)) {}
  constexpr deferred(Data data, DSF sf)
      : data_(std::move(data)), sf_(std::move(sf)) {}

  template <Receiver<is_none<>> Out>
    requires Invocable<DSF&, Data&, Out>
  void submit(Out out) {
    sf_(data_, std::move(out));
  }
};

////////////////////////////////////////////////////////////////////////////////
// make_deferred
inline auto make_deferred() -> deferred<ignoreSF> {
  return deferred<ignoreSF>{};
}
template <class SF>
auto make_deferred(SF sf) -> deferred<SF> {
  return deferred<SF>{std::move(sf)};
}
template <Sender<is_none<>> Wrapped>
auto make_deferred(Wrapped w) ->
    deferred<detail::erase_deferred_t, std::exception_ptr> {
  return deferred<detail::erase_deferred_t, std::exception_ptr>{std::move(w)};
}
template <Sender<is_none<>> Data, class DSF>
auto make_deferred(Data data, DSF sf) -> deferred<Data, DSF> {
  return deferred<Data, DSF>{std::move(data), std::move(sf)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
deferred() -> deferred<ignoreSF>;

template <class SF>
deferred(SF) -> deferred<SF>;

template <Sender<is_none<>> Wrapped>
deferred(Wrapped) ->
    deferred<detail::erase_deferred_t, std::exception_ptr>;

template <Sender<is_none<>> Data, class DSF>
deferred(Data, DSF) -> deferred<Data, DSF>;
#endif

template <class E = std::exception_ptr>
using any_deferred = deferred<detail::erase_deferred_t, E>;

// template <SenderTo<any_none<std::exception_ptr>, is_none<>> Wrapped>
// auto erase_cast(Wrapped w) {
//   return deferred<detail::erase_deferred_t, std::exception_ptr>{std::move(w)};
// }
//
// template <class E, SenderTo<any_none<E>, is_none<>> Wrapped>
//   requires Same<is_none<>, properties_t<Wrapped>>
// auto erase_cast(Wrapped w) {
//   return deferred<detail::erase_deferred_t, E>{std::move(w)};
// }

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <future>
//#include "none.h"

namespace pushmi {

template <class V, class E>
class single<V, E> {
  bool done_ = false;
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(std::promise<int>)]; // can hold a std::promise in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static void s_done(data&) {}
    static void s_error(data&, E) noexcept { std::terminate(); }
    static void s_rvalue(data&, V&&) {}
    static void s_lvalue(data&, V&) {}
    void (*op_)(data&, data*) = s_op;
    void (*done_)(data&) = s_done;
    void (*error_)(data&, E) noexcept = s_error;
    void (*rvalue_)(data&, V&&) = s_rvalue;
    void (*lvalue_)(data&, V&) = s_lvalue;
    static constexpr vtable const noop_ = {};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, single>::value, U>;
  template <class Wrapped>
  static void check() {
    static_assert(Invocable<decltype(::pushmi::set_value), Wrapped, V>,
      "Wrapped single must support values of type V");
    static_assert(NothrowInvocable<decltype(::pushmi::set_error), Wrapped, std::exception_ptr>,
      "Wrapped single must support std::exception_ptr and be noexcept");
    static_assert(NothrowInvocable<decltype(::pushmi::set_error), Wrapped, E>,
      "Wrapped single must support E and be noexcept");
  }
public:
  using properties = property_set<is_receiver<>, is_single<>>;

  single() = default;
  single(single&& that) noexcept : single() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped>
   requires SingleReceiver<wrapped_t<Wrapped>, V, E>
  explicit single(Wrapped obj) : single() {
    check<Wrapped>();
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static void done(data& src) {
        ::pushmi::set_done(*static_cast<Wrapped*>(src.pobj_));
      }
      static void error(data& src, E e) noexcept {
        ::pushmi::set_error(*static_cast<Wrapped*>(src.pobj_), std::move(e));
      }
      static void rvalue(data& src, V&& v) {
        ::pushmi::set_value(*static_cast<Wrapped*>(src.pobj_), (V&&) v);
      }
      static void lvalue(data& src, V& v) {
        ::pushmi::set_value(*static_cast<Wrapped*>(src.pobj_), v);
      }
    };
    static const vtable vtbl{s::op, s::done, s::error, s::rvalue, s::lvalue};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  template <class Wrapped>
   requires SingleReceiver<wrapped_t<Wrapped>, V, E> && insitu<Wrapped>()
  explicit single(Wrapped obj) noexcept : single() {
    check<Wrapped>();
    struct s {
      static void op(data& src, data* dst) {
          if (dst)
            new (dst->buffer_) Wrapped(
                std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
          static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static void done(data& src) {
        ::pushmi::set_done(*static_cast<Wrapped*>((void*)src.buffer_));
      }
      static void error(data& src, E e) noexcept {
        ::pushmi::set_error(
          *static_cast<Wrapped*>((void*)src.buffer_),
          std::move(e));
      }
      static void rvalue(data& src, V&& v) {
        ::pushmi::set_value(*static_cast<Wrapped*>((void*)src.buffer_), (V&&) v);
      }
      static void lvalue(data& src, V& v) {
        ::pushmi::set_value(*static_cast<Wrapped*>((void*)src.buffer_), v);
      }
    };
    static const vtable vtbl{s::op, s::done, s::error, s::rvalue, s::lvalue};
    new ((void*)data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~single() {
    vptr_->op_(data_, nullptr);
  }
  single& operator=(single&& that) noexcept {
    this->~single();
    new ((void*)this) single(std::move(that));
    return *this;
  }
  template<class T>
  requires ConvertibleTo<T&&, V&&>
  void value(T&& t) {
    if (!done_) {
      done_ = true;
      vptr_->rvalue_(data_, (T&&) t);
    }
  }
  template<class T>
  requires ConvertibleTo<T&, V&>
  void value(T& t) {
    if (!done_) {
      done_ = true;
      vptr_->lvalue_(data_, t);
    }
  }
  void error(E e) noexcept {
    if (!done_) {
      done_ = true;
      vptr_->error_(data_, std::move(e));
    }
  }
  void done() {
    if (!done_) {
      done_ = true;
      vptr_->done_(data_);
    }
  }
};

// Class static definitions:
template <class V, class E>
constexpr typename single<V, E>::vtable const single<V, E>::vtable::noop_;

template <class VF, class EF, class DF>
  requires Invocable<DF&>
class single<VF, EF, DF> {
  bool done_ = false;
  VF vf_{};
  EF ef_{};
  DF df_{};

  static_assert(
      !detail::is_v<VF, on_error_fn>,
      "the first parameter is the value implementation, but on_error{} was passed");
  static_assert(
      !detail::is_v<EF, on_value_fn>,
      "the second parameter is the error implementation, but on_value{} was passed");
  static_assert(NothrowInvocable<EF, std::exception_ptr>,
      "error function must be noexcept and support std::exception_ptr");

 public:
  using properties = property_set<is_receiver<>, is_single<>>;

  single() = default;
  constexpr explicit single(VF vf) : single(std::move(vf), EF{}, DF{}) {}
  constexpr explicit single(EF ef) : single(VF{}, std::move(ef), DF{}) {}
  constexpr explicit single(DF df) : single(VF{}, EF{}, std::move(df)) {}
  constexpr single(EF ef, DF df)
      : done_(false), vf_(), ef_(std::move(ef)), df_(std::move(df)) {}
  constexpr single(VF vf, EF ef, DF df = DF{})
      : done_(false), vf_(std::move(vf)), ef_(std::move(ef)), df_(std::move(df))
  {}

  template <class V>
  requires Invocable<VF&, V>
  void value(V&& v) {
    if (done_) {return;}
    done_ = true;
    vf_((V&&) v);
  }
  template <class E>
  requires Invocable<EF&, E>
  void error(E e) noexcept {
    static_assert(NothrowInvocable<EF&, E>, "error function must be noexcept");
    if (!done_) {
      done_ = true;
      ef_(std::move(e));
    }
  }
  void done() {
    if (!done_) {
      done_ = true;
      df_();
    }
  }
};

template <Receiver Data, class DVF, class DEF, class DDF>
requires Invocable<DDF&, Data&> class single<Data, DVF, DEF, DDF> {
  bool done_ = false;
  Data data_{};
  DVF vf_{};
  DEF ef_{};
  DDF df_{};

  static_assert(
      !detail::is_v<DVF, on_error_fn>,
      "the first parameter is the value implementation, but on_error{} was passed");
  static_assert(
      !detail::is_v<DEF, on_value_fn>,
      "the second parameter is the error implementation, but on_value{} was passed");
  static_assert(NothrowInvocable<DEF, Data&, std::exception_ptr>,
      "error function must be noexcept and support std::exception_ptr");

 public:
  using properties = property_set<is_receiver<>, is_single<>>;

  constexpr explicit single(Data d)
      : single(std::move(d), DVF{}, DEF{}, DDF{}) {}
  constexpr single(Data d, DDF df)
      : done_(false), data_(std::move(d)), vf_(), ef_(), df_(df) {}
  constexpr single(Data d, DEF ef, DDF df = DDF{})
      : done_(false), data_(std::move(d)), vf_(), ef_(ef), df_(df) {}
  constexpr single(Data d, DVF vf, DEF ef = DEF{}, DDF df = DDF{})
      : done_(false), data_(std::move(d)), vf_(vf), ef_(ef), df_(df) {}

  template <class V>
  requires Invocable<DVF&, Data&, V>
  void value(V&& v) {
    if (!done_) {
      done_ = true;
      vf_(data_, (V&&) v);
    }
  }
  template <class E>
  requires Invocable<DEF&, Data&, E>
  void error(E e) noexcept {
    static_assert(
        NothrowInvocable<DEF&, Data&, E>, "error function must be noexcept");
    if (!done_) {
      done_ = true;
      ef_(data_, std::move(e));
    }
  }
  void done() {
    if (!done_) {
      done_ = true;
      df_(data_);
    }
  }
};

template <>
class single<>
    : public single<ignoreVF, abortEF, ignoreDF> {
public:
  single() = default;
};

////////////////////////////////////////////////////////////////////////////////
// make_single
inline auto make_single() -> single<> {
  return {};
}
template <class VF>
auto make_single(VF vf) -> single<VF, abortEF, ignoreDF> {
  return single<VF, abortEF, ignoreDF>{std::move(vf)};
}
template <class... EFN>
auto make_single(on_error_fn<EFN...> ef) -> single<ignoreVF, on_error_fn<EFN...>, ignoreDF> {
  return single<ignoreVF, on_error_fn<EFN...>, ignoreDF>{std::move(ef)};
}
template <class DF>
  requires Invocable<DF&>
auto make_single(DF df) -> single<ignoreVF, abortEF, DF> {
  return single<ignoreVF, abortEF, DF>{std::move(df)};
}
template <class VF, class EF>
auto make_single(VF vf, EF ef) -> single<VF, EF, ignoreDF> {
  return {std::move(vf), std::move(ef)};
}
template <class EF, class DF>
  requires Invocable<DF&>
auto make_single(EF ef, DF df) -> single<ignoreVF, EF, DF> {
  return {std::move(ef), std::move(df)};
}
template <class VF, class EF, class DF>
  requires Invocable<DF&>
auto make_single(VF vf, EF ef, DF df) -> single<VF, EF, DF> {
  return {std::move(vf), std::move(ef), std::move(df)};
}
template <Receiver<is_single<>> Data>
auto make_single(Data d) -> single<Data, passDVF, passDEF, passDDF> {
  return single<Data, passDVF, passDEF, passDDF>{std::move(d)};
}
template <Receiver<is_single<>> Data, class DVF>
auto make_single(Data d, DVF vf) -> single<Data, DVF, passDEF, passDDF> {
  return {std::move(d), std::move(vf)};
}
template <Receiver<is_single<>> Data, class... DEFN>
auto make_single(Data d, on_error_fn<DEFN...> ef) ->
    single<Data, passDVF, on_error_fn<DEFN...>, passDDF> {
  return {std::move(d), std::move(ef)};
}
template <Receiver<is_single<>> Data, class DDF>
  requires Invocable<DDF&, Data&>
auto make_single(Data d, DDF df) -> single<Data, passDVF, passDEF, DDF> {
  return {std::move(d), std::move(df)};
}
template <Receiver<is_single<>> Data, class DVF, class DEF>
auto make_single(Data d, DVF vf, DEF ef) -> single<Data, DVF, DEF, passDDF> {
  return {std::move(d), std::move(vf), std::move(ef)};
}
template <Receiver<is_single<>> Data, class DEF, class DDF>
  requires Invocable<DDF&, Data&>
auto make_single(Data d, DEF ef, DDF df) -> single<Data, passDVF, DEF, DDF> {
  return {std::move(d), std::move(ef), std::move(df)};
}
template <Receiver<is_single<>> Data, class DVF, class DEF, class DDF>
auto make_single(Data d, DVF vf, DEF ef, DDF df) -> single<Data, DVF, DEF, DDF> {
  return {std::move(d), std::move(vf), std::move(ef), std::move(df)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
single() -> single<>;

template <class VF>
single(VF) -> single<VF, abortEF, ignoreDF>;

template <class... EFN>
single(on_error_fn<EFN...>) -> single<ignoreVF, on_error_fn<EFN...>, ignoreDF>;

template <class DF>
  requires Invocable<DF&>
single(DF) -> single<ignoreVF, abortEF, DF>;

template <class VF, class EF>
single(VF, EF) -> single<VF, EF, ignoreDF>;

template <class EF, class DF>
  requires Invocable<DF&>
single(EF, DF) -> single<ignoreVF, EF, DF>;

template <class VF, class EF, class DF>
  requires Invocable<DF&>
single(VF, EF, DF) -> single<VF, EF, DF>;

template <Receiver<is_single<>> Data>
single(Data d) -> single<Data, passDVF, passDEF, passDDF>;

template <Receiver<is_single<>> Data, class DVF>
single(Data d, DVF vf) -> single<Data, DVF, passDEF, passDDF>;

template <Receiver<is_single<>> Data, class... DEFN>
single(Data d, on_error_fn<DEFN...>) ->
    single<Data, passDVF, on_error_fn<DEFN...>, passDDF>;

template <Receiver<is_single<>> Data, class DDF>
  requires Invocable<DDF&, Data&>
single(Data d, DDF) -> single<Data, passDVF, passDEF, DDF>;

template <Receiver<is_single<>> Data, class DVF, class DEF>
single(Data d, DVF vf, DEF ef) -> single<Data, DVF, DEF, passDDF>;

template <Receiver<is_single<>> Data, class DEF, class DDF>
  requires Invocable<DDF&, Data&>
single(Data d, DEF, DDF) -> single<Data, passDVF, DEF, DDF>;

template <Receiver<is_single<>> Data, class DVF, class DEF, class DDF>
single(Data d, DVF vf, DEF ef, DDF df) -> single<Data, DVF, DEF, DDF>;
#endif

template <class V, class E = std::exception_ptr>
using any_single = single<V, E>;

template<>
struct construct_deduced<single> {
  template<class... AN>
  auto operator()(AN&&... an) const -> decltype(pushmi::make_single((AN&&) an...)) {
    return pushmi::make_single((AN&&) an...);
  }
};

// template <class V, class E = std::exception_ptr, class Wrapped>
//     requires SingleReceiver<Wrapped, V, E> && !detail::is_v<Wrapped, none>
// auto erase_cast(Wrapped w) {
//   return single<V, E>{std::move(w)};
// }

template<class T, SenderTo<std::promise<T>, is_single<>> Out>
std::future<T> future_from(Out singleSender) {
  std::promise<T> p;
  auto result = p.get_future();
  submit(singleSender, std::move(p));
  return result;
}

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "single.h"

namespace pushmi {

template <class V, class E>
class single_deferred<V, E> {
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(V)]; // can hold a V in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static void s_submit(data&, single<V, E>) {}
    void (*op_)(data&, data*) = s_op;
    void (*submit_)(data&, single<V, E>) = s_submit;
    static constexpr vtable const noop_ {};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, single_deferred>::value, U>;
 public:
  using properties = property_set<is_sender<>, is_single<>>;

  single_deferred() = default;
  single_deferred(single_deferred&& that) noexcept : single_deferred() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }

  template <class Wrapped>
    requires SenderTo<wrapped_t<Wrapped>, single<V, E>, is_single<>>
  explicit single_deferred(Wrapped obj) : single_deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static void submit(data& src, single<V, E> out) {
        ::pushmi::submit(*static_cast<Wrapped*>(src.pobj_), std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::submit};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  template <class Wrapped>
    requires SenderTo<wrapped_t<Wrapped>, single<V, E>, is_single<>> && insitu<Wrapped>()
  explicit single_deferred(Wrapped obj) noexcept : single_deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          new (dst->buffer_) Wrapped(
              std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
        static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static void submit(data& src, single<V, E> out) {
        ::pushmi::submit(
            *static_cast<Wrapped*>((void*)src.buffer_), std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::submit};
    new (data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~single_deferred() {
    vptr_->op_(data_, nullptr);
  }
  single_deferred& operator=(single_deferred&& that) noexcept {
    this->~single_deferred();
    new ((void*)this) single_deferred(std::move(that));
    return *this;
  }
  void submit(single<V, E> out) {
    vptr_->submit_(data_, std::move(out));
  }
};

// Class static definitions:
template <class V, class E>
constexpr typename single_deferred<V, E>::vtable const
    single_deferred<V, E>::vtable::noop_;

template <class SF>
class single_deferred<SF> {
  SF sf_;

 public:
  using properties = property_set<is_sender<>, is_single<>>;

  constexpr single_deferred() = default;
  constexpr explicit single_deferred(SF sf)
      : sf_(std::move(sf)) {}

  template <Receiver<is_single<>> Out>
    requires Invocable<SF&, Out>
  void submit(Out out) {
    sf_(std::move(out));
  }
};

template <Sender<is_single<>> Data, class DSF>
class single_deferred<Data, DSF> {
  Data data_;
  DSF sf_;

 public:
  using properties = property_set<is_sender<>, is_single<>>;

  constexpr single_deferred() = default;
  constexpr explicit single_deferred(Data data)
      : data_(std::move(data)) {}
  constexpr single_deferred(Data data, DSF sf)
      : data_(std::move(data)), sf_(std::move(sf)) {}
  template <Receiver<is_single<>> Out>
    requires Invocable<DSF&, Data&, Out>
  void submit(Out out) {
    sf_(data_, std::move(out));
  }
};

////////////////////////////////////////////////////////////////////////////////
// make_single_deferred
inline auto make_single_deferred() -> single_deferred<ignoreSF> {
  return {};
}
template <class SF>
auto make_single_deferred(SF sf) -> single_deferred<SF> {
  return single_deferred<SF>{std::move(sf)};
}
template <Sender<is_single<>> Data>
auto make_single_deferred(Data d) -> single_deferred<Data, passDSF> {
  return single_deferred<Data, passDSF>{std::move(d)};
}
template <Sender<is_single<>> Data, class DSF>
auto make_single_deferred(Data d, DSF sf) -> single_deferred<Data, DSF> {
  return {std::move(d), std::move(sf)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
single_deferred() -> single_deferred<ignoreSF>;

template <class SF>
single_deferred(SF) -> single_deferred<SF>;

template <Sender<is_single<>> Data>
single_deferred(Data) -> single_deferred<Data, passDSF>;

template <Sender<is_single<>> Data, class DSF>
single_deferred(Data, DSF) -> single_deferred<Data, DSF>;
#endif

template <class V, class E = std::exception_ptr>
using any_single_deferred = single_deferred<V, E>;

// template <
//     class V,
//     class E = std::exception_ptr,
//     SenderTo<single<V, E>, is_single<>> Wrapped>
// auto erase_cast(Wrapped w) {
//   return single_deferred<V, E>{std::move(w)};
// }

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "single.h"

namespace pushmi {

template <class V, class E, class TP>
class time_single_deferred<V, E, TP> {
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(std::promise<int>)]; // can hold a V in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static TP s_now(data&) { return TP{}; }
    static void s_submit(data&, TP, single<V, E>) {}
    void (*op_)(data&, data*) = s_op;
    TP (*now_)(data&) = s_now;
    void (*submit_)(data&, TP, single<V, E>) = s_submit;
    static constexpr vtable const noop_ = {};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, time_single_deferred>::value, U>;

 public:
  using properties = property_set<is_time<>, is_single<>>;

  time_single_deferred() = default;
  time_single_deferred(time_single_deferred&& that) noexcept
      : time_single_deferred() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped, Sender<is_single<>> W = wrapped_t<Wrapped>>
    requires TimeSenderTo<W, single<V, E>>
  explicit time_single_deferred(Wrapped obj) : time_single_deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static TP now(data& src) {
        return ::pushmi::now(*static_cast<Wrapped*>(src.pobj_));
      }
      static void submit(data& src, TP at, single<V, E> out) {
        ::pushmi::submit(
            *static_cast<Wrapped*>(src.pobj_),
            std::move(at),
            std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::now, s::submit};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  template <class Wrapped, Sender<is_single<>> W = wrapped_t<Wrapped>>
    requires TimeSenderTo<W, single<V, E>> && insitu<Wrapped>()
  explicit time_single_deferred(Wrapped obj) noexcept : time_single_deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          new (dst->buffer_) Wrapped(
              std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
        static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static TP now(data& src) {
        return ::pushmi::now(*static_cast<Wrapped*>((void*)src.buffer_));
      }
      static void submit(data& src, TP tp, single<V, E> out) {
        ::pushmi::submit(
            *static_cast<Wrapped*>((void*)src.buffer_),
            std::move(tp),
            std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::now, s::submit};
    new (data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~time_single_deferred() {
    vptr_->op_(data_, nullptr);
  }
  time_single_deferred& operator=(time_single_deferred&& that) noexcept {
    this->~time_single_deferred();
    new ((void*)this) time_single_deferred(std::move(that));
    return *this;
  }
  TP now() {
    vptr_->now_(data_);
  }
  void submit(TP at, single<V, E> out) {
    vptr_->submit_(data_, std::move(at), std::move(out));
  }
};

// Class static definitions:
template <class V, class E, class TP>
constexpr typename time_single_deferred<V, E, TP>::vtable const
    time_single_deferred<V, E, TP>::vtable::noop_;

template <class SF, Invocable NF>
class time_single_deferred<SF, NF> {
  SF sf_{};
  NF nf_{};

 public:
  using properties = property_set<is_time<>, is_single<>>;

  constexpr time_single_deferred() = default;
  constexpr explicit time_single_deferred(SF sf)
      : sf_(std::move(sf)) {}
  constexpr time_single_deferred(SF sf, NF nf)
      : sf_(std::move(sf)), nf_(std::move(nf)) {}
  auto now() {
    return nf_();
  }
  PUSHMI_TEMPLATE(class TP, class Out)
    (requires Regular<TP> && Receiver<Out, is_single<>> &&
      Invocable<SF&, TP, Out>)
  void submit(TP tp, Out out) {
    sf_(std::move(tp), std::move(out));
  }
};

template <TimeSender<is_single<>> Data, class DSF, Invocable<Data&> DNF>
class time_single_deferred<Data, DSF, DNF> {
  Data data_{};
  DSF sf_{};
  DNF nf_{};

 public:
  using properties = property_set<is_time<>, is_single<>>;

  constexpr time_single_deferred() = default;
  constexpr explicit time_single_deferred(Data data)
      : data_(std::move(data)) {}
  constexpr time_single_deferred(Data data, DSF sf, DNF nf = DNF{})
      : data_(std::move(data)), sf_(std::move(sf)), nf_(std::move(nf)) {}
  auto now() {
    return nf_(data_);
  }
  PUSHMI_TEMPLATE(class TP, class Out)
    (requires Regular<TP> && Receiver<Out, is_single<>> &&
      Invocable<DSF&, Data&, TP, Out>)
  void submit(TP tp, Out out) {
    sf_(data_, std::move(tp), std::move(out));
  }
};

////////////////////////////////////////////////////////////////////////////////
// make_time_single_deferred
inline auto make_time_single_deferred() ->
    time_single_deferred<ignoreSF, systemNowF> {
  return {};
}
template <class SF>
auto make_time_single_deferred(SF sf) -> time_single_deferred<SF, systemNowF> {
  return time_single_deferred<SF, systemNowF>{std::move(sf)};
}
template <class SF, Invocable NF>
auto make_time_single_deferred(SF sf, NF nf) -> time_single_deferred<SF, NF> {
  return {std::move(sf), std::move(nf)};
}
template <TimeSender<is_single<>> Data, class DSF>
auto make_time_single_deferred(Data d, DSF sf) ->
    time_single_deferred<Data, DSF, passDNF> {
  return {std::move(d), std::move(sf)};
}
template <TimeSender<is_single<>> Data, class DSF, class DNF>
auto make_time_single_deferred(Data d, DSF sf, DNF nf) ->
    time_single_deferred<Data, DSF, DNF> {
  return {std::move(d), std::move(sf), std::move(nf)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
time_single_deferred() -> time_single_deferred<ignoreSF, systemNowF>;

template <class SF>
time_single_deferred(SF) -> time_single_deferred<SF, systemNowF>;

template <class SF, Invocable NF>
time_single_deferred(SF, NF) -> time_single_deferred<SF, NF>;

template <TimeSender<is_single<>> Data, class DSF>
time_single_deferred(Data, DSF) -> time_single_deferred<Data, DSF, passDNF>;

template <TimeSender<is_single<>> Data, class DSF, class DNF>
time_single_deferred(Data, DSF, DNF) -> time_single_deferred<Data, DSF, DNF>;
#endif

template <
    class V,
    class E = std::exception_ptr,
    class TP = std::chrono::system_clock::time_point>
using any_time_single_deferred = time_single_deferred<V, E, TP>;

// template <
//     class V,
//     class E = std::exception_ptr,
//     class TP = std::chrono::system_clock::time_point,
//     TimeSenderTo<single<V, E>, is_single<>> Wrapped>
// auto erase_cast(Wrapped w) {
//   return time_single_deferred<V, E>{std::move(w)};
// }

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <chrono>
//#include <functional>
//#include "time_single_deferred.h"

namespace pushmi {

namespace detail {
template<class E, class TP>
struct any_time_executor_ref_base {
private:
  friend any_time_executor_ref<E, TP, 0>;
  friend any_time_executor_ref<E, TP, 1>;
  using Other = any_time_executor_ref<E, TP, 1>;

  void* pobj_;
  struct vtable {
    TP (*now_)(void*);
    void (*submit_)(void*, TP, void*);
  } const *vptr_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_base_of<any_time_executor_ref_base, U>::value, U>;
public:
  using properties = property_set<is_time<>, is_single<>>;

  any_time_executor_ref_base() = delete;
  any_time_executor_ref_base(const any_time_executor_ref_base&) = default;

  template <class Wrapped, TimeSender<is_single<>> W = wrapped_t<Wrapped>>
    // requires TimeSenderTo<W, single<Other, E>>
  any_time_executor_ref_base(Wrapped& w) {
    // This can't be a requirement because it asks if submit(w, now(w), single<T,E>)
    // is well-formed (where T is an alias for any_time_executor_ref). If w
    // has a submit that is constrained with SingleReceiver<single<T, E>, T'&, E'>, that
    // will ask whether value(single<T,E>, T'&) is well-formed. And *that* will
    // ask whether T'& is convertible to T. That brings us right back to this
    // constructor. Constraint recursion!
    static_assert(TimeSenderTo<W, single<Other, E>>);
    struct s {
      static TP now(void* pobj) {
        return ::pushmi::now(*static_cast<Wrapped*>(pobj));
      }
      static void submit(void* pobj, TP tp, void* s) {
        return ::pushmi::submit(
          *static_cast<Wrapped*>(pobj),
          tp,
          std::move(*static_cast<single<Other, E>*>(s)));
      }
    };
    static const vtable vtbl{s::now, s::submit};
    pobj_ = std::addressof(w);
    vptr_ = &vtbl;
  }
  std::chrono::system_clock::time_point now() {
    return vptr_->now_(pobj_);
  }
  template<class SingleReceiver>
  void submit(TP tp, SingleReceiver&& sa) {
    // static_assert(
    //   ConvertibleTo<SingleReceiver, any_single<Other, E>>,
    //   "requires any_single<any_time_executor_ref<E, TP>, E>");
    any_single<Other, E> s{(SingleReceiver&&) sa};
    vptr_->submit_(pobj_, tp, &s);
  }
};
} // namespace detail

template<class E, class TP, int i>
struct any_time_executor_ref : detail::any_time_executor_ref_base<E, TP> {
  using detail::any_time_executor_ref_base<E, TP>::any_time_executor_ref_base;
  any_time_executor_ref(const detail::any_time_executor_ref_base<E, TP>& o)
    : detail::any_time_executor_ref_base<E, TP>(o) {}
};

////////////////////////////////////////////////////////////////////////////////
// make_any_time_executor_ref
template <
    class E = std::exception_ptr,
    class TP = std::chrono::system_clock::time_point>
auto make_any_time_executor_ref() -> any_time_executor_ref<E, TP> {
  return any_time_executor_ref<E, TP, 0>{};
}
template <
    class E = std::exception_ptr,
    class TP = std::chrono::system_clock::time_point,
    class Wrapped>
auto make_any_time_executor_ref(Wrapped w) -> any_time_executor_ref<E, TP> {
  return any_time_executor_ref<E, TP, 0>{std::move(w)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
any_time_executor_ref() ->
    any_time_executor_ref<
        std::exception_ptr,
        std::chrono::system_clock::time_point>;

template <class Wrapped>
any_time_executor_ref(Wrapped) ->
    any_time_executor_ref<
        std::exception_ptr,
        std::chrono::system_clock::time_point>;
#endif

template<class E, class TP>
struct any_time_executor :
  any_time_single_deferred<any_time_executor_ref<E, TP>, E, TP> {
  constexpr any_time_executor() = default;
  using any_time_single_deferred<any_time_executor_ref<E, TP>, E, TP>::
    any_time_single_deferred;
};

////////////////////////////////////////////////////////////////////////////////
// make_any_time_executor
template <
    class E = std::exception_ptr,
    class TP = std::chrono::system_clock::time_point>
auto make_any_time_executor() -> any_time_executor<E, TP> {
  return any_time_executor<E, TP>{};
}

template <
    class E = std::exception_ptr,
    class TP = std::chrono::system_clock::time_point,
    class Wrapped>
auto make_any_time_executor(Wrapped w) -> any_time_executor<E, TP> {
  return any_time_executor<E, TP>{std::move(w)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
any_time_executor() ->
    any_time_executor<
        std::exception_ptr,
        std::chrono::system_clock::time_point>;

template <class Wrapped>
any_time_executor(Wrapped) ->
    any_time_executor<
        std::exception_ptr, 
        std::chrono::system_clock::time_point>;
#endif

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "single.h"

namespace pushmi {

template <class V, class PE, class E>
class flow_single<V, PE, E> {
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(std::promise<int>)]; // can hold a std::promise in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static void s_done(data&) {}
    static void s_error(data&, E) noexcept { std::terminate(); }
    static void s_value(data&, V) {}
    static void s_stopping(data&) noexcept {}
    static void s_starting(data&, any_none<PE>&) {}
    void (*op_)(data&, data*) = s_op;
    void (*done_)(data&) = s_done;
    void (*error_)(data&, E) noexcept = s_error;
    void (*value_)(data&, V) = s_value;
    void (*stopping_)(data&) noexcept = s_stopping;
    void (*starting_)(data&, any_none<PE>&) = s_starting;
    static constexpr vtable const noop_ {};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, flow_single>::value, U>;
public:
  using properties = property_set<is_receiver<>, is_flow<>, is_single<>>;

  flow_single() = default;
  flow_single(flow_single&& that) noexcept : flow_single() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped>
    requires FlowSingleReceiver<wrapped_t<Wrapped>, any_none<PE>, V, PE, E>
  explicit flow_single(Wrapped obj) : flow_single() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static void done(data& src) {
        ::pushmi::set_done(*static_cast<Wrapped*>(src.pobj_));
      }
      static void error(data& src, E e) noexcept {
        ::pushmi::set_error(*static_cast<Wrapped*>(src.pobj_), std::move(e));
      }
      static void value(data& src, V v) {
        ::pushmi::set_value(*static_cast<Wrapped*>(src.pobj_), std::move(v));
      }
      static void stopping(data& src) {
        ::pushmi::set_stopping(*static_cast<Wrapped*>(src.pobj_));
      }
      static void stopping(data& src, any_none<PE>& up) {
        ::pushmi::set_starting(*static_cast<Wrapped*>(src.pobj_), up);
      }
    };
    static const vtable vtbl{s::op, s::done, s::error, s::value, s::stopping, s::starting};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  template <class Wrapped>
    requires FlowSingleReceiver<wrapped_t<Wrapped>, any_none<PE>, V, PE, E> && insitu<Wrapped>()
  explicit flow_single(Wrapped obj) noexcept : flow_single() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          new (dst->buffer_) Wrapped(
              std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
        static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static void done(data& src) {
        ::pushmi::set_done(*static_cast<Wrapped*>((void*)src.buffer_));
      }
      static void error(data& src, E e) noexcept {::pushmi::set_error(
          *static_cast<Wrapped*>((void*)src.buffer_),
          std::move(e));
      }
      static void value(data& src, V v) {
        ::pushmi::set_value(
            *static_cast<Wrapped*>((void*)src.buffer_), std::move(v));
      }
      static void stopping(data& src) noexcept {
        ::pushmi::set_stopping(*static_cast<Wrapped*>((void*)src.buffer_));
      }
      static void starting(data& src, any_none<PE>& up) {
        ::pushmi::set_starting(*static_cast<Wrapped*>((void*)src.buffer_), up);
      }
    };
    static const vtable vtbl{s::op, s::done, s::error, s::value, s::stopping, s::starting};
    new (data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~flow_single() {
    vptr_->op_(data_, nullptr);
  }
  flow_single& operator=(flow_single&& that) noexcept {
    this->~flow_single();
    new ((void*)this) flow_single(std::move(that));
    return *this;
  }
  void value(V v) {
    vptr_->value_(data_, std::move(v));
  }
  void error(E e) noexcept {
    vptr_->error_(data_, std::move(e));
  }
  void done() {
    vptr_->done_(data_);
  }

  void stopping() noexcept {
    vptr_->stopping_(data_);
  }
  void starting(any_none<PE>& up) {
    vptr_->starting_(data_, up);
  }
};

// Class static definitions:
template <class V, class PE, class E>
constexpr typename flow_single<V, PE, E>::vtable const
    flow_single<V, PE, E>::vtable::noop_;

template <class VF, class EF, class DF, class StpF, class StrtF>
  requires Invocable<DF&>
class flow_single<VF, EF, DF, StpF, StrtF> {
  VF vf_;
  EF ef_;
  DF df_;
  StpF stpf_;
  StrtF strtf_;

 public:
  using properties = property_set<is_receiver<>, is_flow<>, is_single<>>;

  static_assert(
      !detail::is_v<VF, on_error_fn>,
      "the first parameter is the value implementation, but on_error{} was passed");
  static_assert(
      !detail::is_v<EF, on_value_fn>,
      "the second parameter is the error implementation, but on_value{} was passed");

  flow_single() = default;
  constexpr explicit flow_single(VF vf)
      : flow_single(std::move(vf), EF{}, DF{}) {}
  constexpr explicit flow_single(EF ef)
      : flow_single(VF{}, std::move(ef), DF{}) {}
  constexpr explicit flow_single(DF df)
      : flow_single(VF{}, EF{}, std::move(df)) {}
  constexpr flow_single(EF ef, DF df)
      : vf_(), ef_(std::move(ef)), df_(std::move(df)) {}
  constexpr flow_single(
      VF vf,
      EF ef,
      DF df = DF{},
      StpF stpf = StpF{},
      StrtF strtf = StrtF{})
      : vf_(std::move(vf)),
        ef_(std::move(ef)),
        df_(std::move(df)),
        stpf_(std::move(stpf)),
        strtf_(std::move(strtf)) {}
  template <class V>
  requires Invocable<VF&, V>
  void value(V v) {
    vf_(v);
  }
  template <class E>
    requires Invocable<EF&, E>
  void error(E e) noexcept {
    static_assert(NothrowInvocable<EF&, E>, "error function must be noexcept");
    ef_(std::move(e));
  }
  void done() {
    df_();
  }
  void stopping() noexcept {
    stpf_();
  }
  template <Receiver<is_none<>> Up>
    requires Invocable<StrtF&, Up&>
  void starting(Up& up) {
    strtf_(up);
  }
};

template <
    Receiver Data,
    class DVF,
    class DEF,
    class DDF,
    class DStpF,
    class DStrtF>
requires Invocable<DDF&, Data&>
class flow_single<Data, DVF, DEF, DDF, DStpF, DStrtF> {
  Data data_;
  DVF vf_;
  DEF ef_;
  DDF df_;
  DStpF stpf_;
  DStrtF strtf_;

 public:
  using properties = property_set<is_receiver<>, is_flow<>, is_single<>>;

  static_assert(
      !detail::is_v<DVF, on_error_fn>,
      "the first parameter is the value implementation, but on_error{} was passed");
  static_assert(
      !detail::is_v<DEF, on_value_fn>,
      "the second parameter is the error implementation, but on_value{} was passed");

  constexpr explicit flow_single(Data d)
      : flow_single(std::move(d), DVF{}, DEF{}, DDF{}) {}
  constexpr flow_single(Data d, DDF df)
      : data_(std::move(d)), vf_(), ef_(), df_(df) {}
  constexpr flow_single(Data d, DEF ef, DDF df = DDF{})
      : data_(std::move(d)), vf_(), ef_(ef), df_(df) {}
  constexpr flow_single(
      Data d,
      DVF vf,
      DEF ef = DEF{},
      DDF df = DDF{},
      DStpF stpf = DStpF{},
      DStrtF strtf = DStrtF{})
      : data_(std::move(d)),
        vf_(vf),
        ef_(ef),
        df_(df),
        stpf_(std::move(stpf)),
        strtf_(std::move(strtf)) {}
  template <class V>
  requires Invocable<DVF&, Data&, V> void value(V v) {
    vf_(data_, v);
  }
  template <class E>
  requires Invocable<DEF&, Data&, E> void error(E e) noexcept {
    static_assert(
        NothrowInvocable<DEF&, Data&, E>, "error function must be noexcept");
    ef_(data_, e);
  }
  void done() {
    df_(data_);
  }
  void stopping() noexcept {
    stpf_(data_);
  }
  template <class Up>
  requires Invocable<DStrtF&, Data&, Up&>
  void starting(Up& up) {
    strtf_(data_, up);
  }
};

template <>
class flow_single<>
    : public flow_single<ignoreVF, abortEF, ignoreDF, ignoreStpF, ignoreStrtF> {
};

// TODO winnow down the number of make_flow_single overloads and deduction
// guides here, as was done for make_single.

////////////////////////////////////////////////////////////////////////////////
// make_flow_single
inline auto make_flow_single() -> flow_single<> {
  return flow_single<>{};
}
template <class VF>
    requires !Receiver<VF> && !detail::is_v<VF, on_error_fn> &&
    !detail::is_v<VF, on_done_fn>
auto make_flow_single(VF vf)
         -> flow_single<VF, abortEF, ignoreDF, ignoreStpF, ignoreStrtF> {
  return flow_single<VF, abortEF, ignoreDF, ignoreStpF, ignoreStrtF>{std::move(vf)};
}
template <class... EFN>
auto make_flow_single(on_error_fn<EFN...> ef)
    -> flow_single<
        ignoreVF,
        on_error_fn<EFN...>,
        ignoreDF,
        ignoreStpF,
        ignoreStrtF> {
  return flow_single<
        ignoreVF,
        on_error_fn<EFN...>,
        ignoreDF,
        ignoreStpF,
        ignoreStrtF>{std::move(ef)};
}
template <class DF>
auto make_flow_single(on_done_fn<DF> df)
    -> flow_single<ignoreVF, abortEF, on_done_fn<DF>, ignoreStpF, ignoreStrtF> {
  return flow_single<ignoreVF, abortEF, on_done_fn<DF>, ignoreStpF, ignoreStrtF>{
      std::move(df)};
}
template <class V, class PE, class E, class Wrapped>
    requires FlowSingleReceiver<Wrapped, V, PE, E> &&
    !detail::is_v<Wrapped, none>
auto make_flow_single(Wrapped w) -> flow_single<V, PE, E> {
  return flow_single<V, PE, E>{std::move(w)};
}
template <class VF, class EF>
    requires !Receiver<VF> && !detail::is_v<VF, on_error_fn> &&
    !detail::is_v<VF, on_done_fn> && !detail::is_v<EF, on_value_fn> &&
    !detail::is_v<EF, on_done_fn>
auto make_flow_single(VF vf, EF ef)
         -> flow_single<VF, EF, ignoreDF, ignoreStpF, ignoreStrtF> {
  return {std::move(vf), std::move(ef)};
}
template <class... EFN, class DF>
auto make_flow_single(on_error_fn<EFN...> ef, on_done_fn<DF> df)
    -> flow_single<
        ignoreVF,
        on_error_fn<EFN...>,
        on_done_fn<DF>,
        ignoreStpF,
        ignoreStrtF> {
  return {std::move(ef), std::move(df)};
}
template <class VF, class EF, class DF>
requires Invocable<DF&>
auto make_flow_single(VF vf, EF ef, DF df)
    -> flow_single<VF, EF, DF, ignoreStpF, ignoreStrtF> {
  return {std::move(vf), std::move(ef), std::move(df)};
}
template <class VF, class EF, class DF, class StpF>
requires Invocable<DF&>&& Invocable<StpF&>
auto make_flow_single(VF vf, EF ef, DF df, StpF stpf)
    -> flow_single<VF, EF, DF, StpF, ignoreStrtF> {
  return {std::move(vf), std::move(ef), std::move(df), std::move(stpf)};
}
template <class VF, class EF, class DF, class StpF, class StrtF>
requires Invocable<DF&>&& Invocable<StpF&>
auto make_flow_single(VF vf, EF ef, DF df, StpF stpf, StrtF strtf)
    -> flow_single<VF, EF, DF, StpF, StrtF> {
  return {std::move(vf), std::move(ef), std::move(df), std::move(stpf), std::move(strtf)};
}
template <Receiver Data>
auto make_flow_single(Data d)
    -> flow_single<Data, passDVF, passDEF, passDDF, passDStpF, passDStrtF> {
  return flow_single<Data, passDVF, passDEF, passDDF, passDStpF, passDStrtF>{
      std::move(d)};
}
template <Receiver Data, class DVF>
    requires !detail::is_v<DVF, on_error_fn> &&
    !detail::is_v<DVF, on_done_fn>
auto make_flow_single(Data d, DVF vf)
         -> flow_single<Data, DVF, passDEF, passDDF, passDStpF, passDStrtF> {
  return {std::move(d), std::move(vf)};
}
template <Receiver Data, class... DEFN>
auto make_flow_single(Data d, on_error_fn<DEFN...> ef)
    -> flow_single<
        Data,
        passDVF,
        on_error_fn<DEFN...>,
        passDDF,
        passDStpF,
        passDStrtF> {
  return {std::move(d), std::move(ef)};
}
template <Receiver Data, class DVF, class DEF>
    requires !detail::is_v<DVF, on_error_fn> && !detail::is_v<DVF, on_done_fn> &&
    !detail::is_v<DEF, on_done_fn>
auto make_flow_single(Data d, DVF vf, DEF ef)
         -> flow_single<Data, DVF, DEF, passDDF, passDStpF, passDStrtF> {
  return {std::move(d), std::move(vf), std::move(ef)};
}
template <Receiver Data, class... DEFN, class DDF>
auto make_flow_single(Data d, on_error_fn<DEFN...> ef, on_done_fn<DDF> df)
    -> flow_single<
        Data,
        passDVF,
        on_error_fn<DEFN...>,
        on_done_fn<DDF>,
        passDStpF,
        passDStrtF> {
  return {std::move(d), std::move(ef), std::move(df)};
}
template <Receiver Data, class DDF>
auto make_flow_single(Data d, on_done_fn<DDF> df)
    -> flow_single<Data, passDVF, passDEF, on_done_fn<DDF>, passDStpF, passDStrtF> {
  return {std::move(d), std::move(df)};
}
template <Receiver Data, class DVF, class DEF, class DDF>
requires Invocable<DDF&, Data&>
auto make_flow_single(Data d, DVF vf, DEF ef, DDF df)
    -> flow_single<Data, DVF, DEF, DDF, passDStpF, passDStrtF> {
  return {std::move(d), std::move(vf), std::move(ef), std::move(df)};
}
template <Receiver Data, class DVF, class DEF, class DDF, class DStpF>
requires Invocable<DDF&, Data&>&& Invocable<DStpF&, Data&>
auto make_flow_single(Data d, DVF vf, DEF ef, DDF df, DStpF stpf)
    -> flow_single<Data, DVF, DEF, DDF, DStpF, passDStrtF> {
  return {std::move(d), std::move(vf), std::move(ef), std::move(df), std::move(stpf)};
}
template <
    Receiver Data,
    class DVF,
    class DEF,
    class DDF,
    class DStpF,
    class DStrtF>
requires Invocable<DDF&, Data&>&& Invocable<DStpF&, Data&>
auto make_flow_single(Data d, DVF vf, DEF ef, DDF df, DStpF stpf, DStrtF strtf)
    -> flow_single<Data, DVF, DEF, DDF, DStpF, DStrtF> {
  return {std::move(d), std::move(vf), std::move(ef), std::move(df), std::move(stpf), std::move(strtf)};
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
flow_single() -> flow_single<>;

template <class VF>
    requires !Receiver<VF> && !detail::is_v<VF, on_error_fn> &&
    !detail::is_v<VF, on_done_fn> flow_single(VF)
         -> flow_single<VF, abortEF, ignoreDF, ignoreStpF, ignoreStrtF>;

template <class... EFN>
flow_single(on_error_fn<EFN...>)
    -> flow_single<
        ignoreVF,
        on_error_fn<EFN...>,
        ignoreDF,
        ignoreStpF,
        ignoreStrtF>;

template <class DF>
flow_single(on_done_fn<DF>)
    -> flow_single<ignoreVF, abortEF, on_done_fn<DF>, ignoreStpF, ignoreStrtF>;

template <class V, class PE, class E, class Wrapped>
    requires FlowSingleReceiver<Wrapped, V, PE, E> &&
    !detail::is_v<Wrapped, none> flow_single(Wrapped) -> flow_single<V, PE, E>;

template <class VF, class EF>
    requires !Receiver<VF> && !detail::is_v<VF, on_error_fn> &&
    !detail::is_v<VF, on_done_fn> && !detail::is_v<EF, on_value_fn> &&
    !detail::is_v<EF, on_done_fn> flow_single(VF, EF)
         -> flow_single<VF, EF, ignoreDF, ignoreStpF, ignoreStrtF>;

template <class... EFN, class DF>
flow_single(on_error_fn<EFN...>, on_done_fn<DF>)
    -> flow_single<
        ignoreVF,
        on_error_fn<EFN...>,
        on_done_fn<DF>,
        ignoreStpF,
        ignoreStrtF>;

template <class VF, class EF, class DF>
requires Invocable<DF&> flow_single(VF, EF, DF)
    -> flow_single<VF, EF, DF, ignoreStpF, ignoreStrtF>;

template <class VF, class EF, class DF, class StpF>
requires Invocable<DF&>&& Invocable<StpF&> flow_single(VF, EF, DF, StpF)
    -> flow_single<VF, EF, DF, StpF, ignoreStrtF>;

template <class VF, class EF, class DF, class StpF, class StrtF>
requires Invocable<DF&>&& Invocable<StpF&> flow_single(VF, EF, DF, StpF, StrtF)
    -> flow_single<VF, EF, DF, StpF, StrtF>;

template <Receiver Data>
flow_single(Data d)
    -> flow_single<Data, passDVF, passDEF, passDDF, passDStpF, passDStrtF>;

template <Receiver Data, class DVF>
    requires !detail::is_v<DVF, on_error_fn> &&
    !detail::is_v<DVF, on_done_fn> flow_single(Data d, DVF vf)
         -> flow_single<Data, DVF, passDEF, passDDF, passDStpF, passDStrtF>;

template <Receiver Data, class... DEFN>
flow_single(Data d, on_error_fn<DEFN...>)
    -> flow_single<
        Data,
        passDVF,
        on_error_fn<DEFN...>,
        passDDF,
        passDStpF,
        passDStrtF>;

template <Receiver Data, class DVF, class DEF>
    requires !detail::is_v<DVF, on_error_fn> && !detail::is_v<DVF, on_done_fn> &&
    !detail::is_v<DEF, on_done_fn> flow_single(Data d, DVF vf, DEF ef)
         -> flow_single<Data, DVF, DEF, passDDF, passDStpF, passDStrtF>;

template <Receiver Data, class... DEFN, class DDF>
flow_single(Data d, on_error_fn<DEFN...>, on_done_fn<DDF>)
    -> flow_single<
        Data,
        passDVF,
        on_error_fn<DEFN...>,
        on_done_fn<DDF>,
        passDStpF,
        passDStrtF>;

template <Receiver Data, class DDF>
flow_single(Data d, on_done_fn<DDF>)
    -> flow_single<Data, passDVF, passDEF, on_done_fn<DDF>, passDStpF, passDStrtF>;

template <Receiver Data, class DVF, class DEF, class DDF>
requires Invocable<DDF&, Data&> flow_single(Data d, DVF vf, DEF ef, DDF df)
    -> flow_single<Data, DVF, DEF, DDF, passDStpF, passDStrtF>;

template <Receiver Data, class DVF, class DEF, class DDF, class DStpF>
requires Invocable<DDF&, Data&>&& Invocable<DStpF&, Data&>
flow_single(Data d, DVF vf, DEF ef, DDF df, DStpF stpf)
    -> flow_single<Data, DVF, DEF, DDF, DStpF, passDStrtF>;

template <
    Receiver Data,
    class DVF,
    class DEF,
    class DDF,
    class DStpF,
    class DStrtF>
requires Invocable<DDF&, Data&>&& Invocable<DStpF&, Data&>
flow_single(Data d, DVF vf, DEF ef, DDF df, DStpF stpf, DStrtF strtf)
    -> flow_single<Data, DVF, DEF, DDF, DStpF, DStrtF>;
#endif

template <class V, class PE = std::exception_ptr, class E = PE>
using any_flow_single = flow_single<V, PE, E>;

// template <class V, class PE = std::exception_ptr, class E = PE, class Wrapped>
//     requires FlowSingleReceiver<Wrapped, V, PE, E> && !detail::is_v<Wrapped, none> &&
//     !detail::is_v<Wrapped, std::promise>
//     auto erase_cast(Wrapped w) {
//   return flow_single<V, PE, E>{std::move(w)};
// }

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "flow_single.h"

namespace pushmi {

template <class V, class PE, class E>
class flow_single_deferred<V, PE, E> {
  union data {
    void* pobj_ = nullptr;
    char buffer_[sizeof(V)]; // can hold a V in-situ
  } data_{};
  template <class Wrapped>
  static constexpr bool insitu() {
    return sizeof(Wrapped) <= sizeof(data::buffer_) &&
        std::is_nothrow_move_constructible<Wrapped>::value;
  }
  struct vtable {
    static void s_op(data&, data*) {}
    static void s_submit(data&, flow_single<V, PE, E>) {}
    void (*op_)(data&, data*) = s_op;
    void (*submit_)(data&, flow_single<V, PE, E>) = s_submit;
    static constexpr vtable const noop_ {};
  } const* vptr_ = &vtable::noop_;
  template <class T, class U = std::decay_t<T>>
  using wrapped_t =
    std::enable_if_t<!std::is_same<U, flow_single_deferred>::value, U>;
 public:
  using properties = property_set<is_sender<>, is_flow<>, is_single<>>;

  flow_single_deferred() = default;
  flow_single_deferred(flow_single_deferred&& that) noexcept
      : flow_single_deferred() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <
      class Wrapped,
      FlowSender<is_single<>> W = wrapped_t<Wrapped>>
  explicit flow_single_deferred(Wrapped obj) : flow_single_deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          dst->pobj_ = std::exchange(src.pobj_, nullptr);
        delete static_cast<Wrapped const*>(src.pobj_);
      }
      static void submit(data& src, flow_single<V, PE, E> out) {
        ::pushmi::submit(*static_cast<Wrapped*>(src.pobj_), std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::submit};
    data_.pobj_ = new Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  template <
      class Wrapped,
      FlowSender<is_single<>> W = wrapped_t<Wrapped>>
    requires insitu<Wrapped>()
  explicit flow_single_deferred(Wrapped obj) noexcept : flow_single_deferred() {
    struct s {
      static void op(data& src, data* dst) {
        if (dst)
          new (dst->buffer_) Wrapped(
              std::move(*static_cast<Wrapped*>((void*)src.buffer_)));
        static_cast<Wrapped const*>((void*)src.buffer_)->~Wrapped();
      }
      static void submit(data& src, flow_single<V, PE, E> out) {
        ::pushmi::submit(
            *static_cast<Wrapped*>((void*)src.buffer_),
            std::move(out));
      }
    };
    static const vtable vtbl{s::op, s::submit};
    new (data_.buffer_) Wrapped(std::move(obj));
    vptr_ = &vtbl;
  }
  ~flow_single_deferred() {
    vptr_->op_(data_, nullptr);
  }
  flow_single_deferred& operator=(flow_single_deferred&& that) noexcept {
    this->~flow_single_deferred();
    new ((void*)this) flow_single_deferred(std::move(that));
    return *this;
  }
  void submit(flow_single<V, PE, E> out) {
    vptr_->submit_(data_, std::move(out));
  }
};

// Class static definitions:
template <class V, class PE, class E>
constexpr typename flow_single_deferred<V, PE, E>::vtable const
    flow_single_deferred<V, PE, E>::vtable::noop_;

template <class SF>
class flow_single_deferred<SF> {
  SF sf_;

 public:
  using properties = property_set<is_sender<>, is_flow<>, is_single<>>;

  constexpr flow_single_deferred() = default;
  constexpr explicit flow_single_deferred(SF sf)
      : sf_(std::move(sf)) {}

  template <Receiver<is_flow<>> Out>
    requires Invocable<SF&, Out>
  void submit(Out out) {
    sf_(std::move(out));
  }
};

////////////////////////////////////////////////////////////////////////////////
// make_flow_single_deferred
inline auto make_flow_single_deferred() -> flow_single_deferred<ignoreSF> {
  return flow_single_deferred<ignoreSF>{};
}
template <class SF>
auto make_flow_single_deferred(SF sf) -> flow_single_deferred<SF> {
  return flow_single_deferred<SF>(std::move(sf));
}

////////////////////////////////////////////////////////////////////////////////
// deduction guides
#if __cpp_deduction_guides >= 201703
flow_single_deferred() -> flow_single_deferred<ignoreSF>;

template <class SF>
flow_single_deferred(SF) -> flow_single_deferred<SF>;
#endif

template <class V, class PE = std::exception_ptr, class E = PE>
using any_flow_single_deferred = flow_single_deferred<V, PE, E>;

// // TODO constrain me
// template <class V, class E = std::exception_ptr, Sender Wrapped>
// auto erase_cast(Wrapped w) {
//   return flow_single_deferred<V, E>{std::move(w)};
// }

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <algorithm>
//#include <chrono>
//#include <deque>
//#include <thread>
//#include "executor.h"
//#include "time_single_deferred.h"

namespace pushmi {

struct recurse_t {};
constexpr const recurse_t recurse{};

namespace detail {

PUSHMI_INLINE_VAR constexpr struct ownordelegate_t {} const ownordelegate {};
PUSHMI_INLINE_VAR constexpr struct ownornest_t {} const ownornest {};

class trampoline_id {
  std::thread::id threadid;
  uintptr_t trampolineid;

 public:
  template <class T>
  explicit trampoline_id(T* trampoline)
      : threadid(std::this_thread::get_id()), trampolineid(trampoline) {}
};

template <class E = std::exception_ptr>
class trampoline {
 public:
  using time_point = std::chrono::system_clock::time_point;

 private:
  using error_type = std::decay_t<E>;
  using work_type =
     any_single<any_time_executor_ref<error_type, time_point>, error_type>;
  using queue_type = std::deque<std::tuple<time_point, work_type>>;
  using pending_type = std::tuple<int, queue_type, time_point>;

  inline static pending_type*& owner() {
    static thread_local pending_type* pending = nullptr;
    return pending;
  }

  inline static int& depth(pending_type& p) {
    return std::get<0>(p);
  }

  inline static queue_type& pending(pending_type& p) {
    return std::get<1>(p);
  }

  inline static time_point& next(pending_type& p) {
    return std::get<2>(p);
  }

 public:
  inline static trampoline_id get_id() {
    return {owner()};
  }

  inline static bool is_owned() {
    return owner() != nullptr;
  }

  inline static time_point now() {
    return std::chrono::system_clock::now();
  }

  template <class Selector, class Derived>
  static void submit(Selector, Derived&, time_point awhen, recurse_t) {
    if (!is_owned()) {
      abort();
    }
    next(*owner()) = awhen;
  }

  template <class SingleReceiver>
    requires !Same<SingleReceiver, recurse_t>
  static void submit(ownordelegate_t, time_point awhen, SingleReceiver awhat);

  template <class SingleReceiver>
    requires !Same<SingleReceiver, recurse_t>
  static void submit(ownornest_t, time_point awhen, SingleReceiver awhat);
};

template <class E = std::exception_ptr>
class delegator {
  using time_point = typename trampoline<E>::time_point;

 public:
  using properties = property_set<is_time<>, is_single<>>;

  time_point now() {
    return trampoline<E>::now();
  }

  template <class SingleReceiver>
    requires Receiver<remove_cvref_t<SingleReceiver>, is_single<>>
  void submit(time_point when, SingleReceiver&& what) {
    trampoline<E>::submit(
        ownordelegate, when, std::forward<SingleReceiver>(what));
  }
};

template <class E = std::exception_ptr>
class nester {
  using time_point = typename trampoline<E>::time_point;

 public:
  using properties = property_set<is_time<>, is_single<>>;

  time_point now() {
    return trampoline<E>::now();
  }

  template <class SingleReceiver>
  void submit(time_point when, SingleReceiver&& what) {
    trampoline<E>::submit(ownornest, when, std::forward<SingleReceiver>(what));
  }
};

template <class E>
template <class SingleReceiver>
  requires !Same<SingleReceiver, recurse_t>
// static
void trampoline<E>::submit(
    ownordelegate_t,
    time_point awhen,
    SingleReceiver awhat) {

  delegator<E> that;

  if (is_owned()) {
    // thread already owned

    // poor mans scope guard
    try {
      if (++depth(*owner()) > 100 || awhen > trampoline<E>::now()) {
        // defer work to owner
        pending(*owner()).push_back(
            std::make_tuple(awhen, work_type{std::move(awhat)}));
      } else {
        // dynamic recursion - optimization to balance queueing and
        // stack usage and value interleaving on the same thread.
        ::pushmi::set_value(awhat, that);
      }
    } catch(...) {
      --depth(*owner());
      throw;
    }
    --depth(*owner());
    return;
  }

  // take over the thread

  pending_type pending_store;
  owner() = &pending_store;
  depth(pending_store) = 0;
  // poor mans scope guard
  try {
    trampoline<E>::submit(ownornest, awhen, std::move(awhat));
  } catch(...) {

    // ignore exceptions while delivering the exception
    try {
      ::pushmi::set_error(awhat, std::current_exception());
      for (auto& item : pending(pending_store)) {
        auto& what = std::get<1>(item);
        ::pushmi::set_error(what, std::current_exception());
      }
    } catch (...) {
    }
    pending(pending_store).clear();

    if(!is_owned()) { std::abort(); }
    if(!pending(pending_store).empty()) { std::abort(); }
    owner() = nullptr;
    throw;
  }
  if(!is_owned()) { std::abort(); }
  if(!pending(pending_store).empty()) { std::abort(); }
  owner() = nullptr;
}

template <class E>
template <class SingleReceiver>
  requires !Same<SingleReceiver, recurse_t>
// static
void trampoline<E>::submit(
    ownornest_t,
    time_point awhen,
    SingleReceiver awhat) {

    delegator<E> that;

  if (!is_owned()) {
    trampoline<E>::submit(ownordelegate, awhen, std::move(awhat));
    return;
  }

  auto& pending_store = *owner();

  // static recursion - tail call optimization
  if (pending(pending_store).empty()) {
    auto when = awhen;
    while (when != time_point{}) {
      if (when > trampoline<E>::now()) {
        std::this_thread::sleep_until(when);
      }
      next(pending_store) = time_point{};
      ::pushmi::set_value(awhat, that);
      when = next(pending_store);
    }
  } else {
    // ensure work is sorted by time
    pending(pending_store)
        .push_back(std::make_tuple(awhen, work_type{std::move(awhat)}));
  }

  if (pending(pending_store).empty()) {
    return;
  }

  while (!pending(pending_store).empty()) {
    std::stable_sort(
        pending(pending_store).begin(),
        pending(pending_store).end(),
        [](auto& lhs, auto& rhs) {
          auto& lwhen = std::get<0>(lhs);
          auto& rwhen = std::get<0>(rhs);
          return lwhen < rwhen;
        });
    auto item = std::move(pending(pending_store).front());
    pending(pending_store).pop_front();
    auto& when = std::get<0>(item);
    auto& what = std::get<1>(item);
    any_time_executor_ref<error_type, time_point> anythis{that};
    ::pushmi::set_value(what, anythis);
  }
}

} // namespace detail

template <class E = std::exception_ptr>
detail::trampoline_id get_trampoline_id() {
  if(!detail::trampoline<E>::is_owned()) { std::abort(); }
  return detail::trampoline<E>::get_id();
}

template <class E = std::exception_ptr>
bool owned_by_trampoline() {
  return detail::trampoline<E>::is_owned();
}

template <class E = std::exception_ptr>
inline detail::delegator<E> trampoline() {
  return {};
}
template <class E = std::exception_ptr>
inline detail::nester<E> nested_trampoline() {
  return {};
}

namespace detail {

template <class E>
  requires TimeSenderTo<delegator<E>, recurse_t>
decltype(auto) repeat(delegator<E>& exec) {
  ::pushmi::submit(exec, ::pushmi::now(exec), recurse);
}
template <class AnyExec>
void repeat(AnyExec& exec) {
  std::abort();
}

} // namespace detail

inline auto repeat() {
  return [](auto& exec) { detail::repeat(exec); };
}

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "executor.h"
//#include "trampoline.h"

namespace pushmi {

// very poor perf example executor.
//

struct __new_thread_submit {
  PUSHMI_TEMPLATE(class TP, class Out)
    (requires Regular<TP> && Receiver<Out>)
  void operator()(TP at, Out out) const {
    std::thread t{[at = std::move(at), out = std::move(out)]() mutable {
      auto tr = trampoline();
      ::pushmi::submit(tr, std::move(at), std::move(out));
    }};
    // pass ownership of thread to out
    t.detach();
  }
};

inline auto new_thread() {
  return make_time_single_deferred(__new_thread_submit{});
}

}
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <meta/meta.hpp>

//#include "../deferred.h"
//#include "../single_deferred.h"
//#include "../detail/functional.h"

namespace pushmi {

namespace operators {

template <class V>
auto empty() {
  return make_single_deferred(
    constrain<mock::SingleReceiver<_1, V>>([](auto out) mutable {
      ::pushmi::set_done(out);
    })
  );
}

inline auto empty() {
  return make_deferred(
    constrain<mock::NoneReceiver<_1>>([](auto out) mutable {
      ::pushmi::set_done(out);
    })
  );
}

} // namespace operators
} // namespace pushmi
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <tuple>
//#include "../piping.h"
//#include "../boosters.h"
//#include "../single.h"
//#include "../detail/if_constexpr.h"
//#include "../detail/functional.h"

namespace pushmi {

#if __cpp_lib_apply	>= 201603
using std::apply;
#else
namespace detail {
  template <class F, class Tuple, std::size_t... Is>
    requires requires (F&& f, Tuple&& t) {
      pushmi::invoke((F&&) f, std::get<Is>((Tuple&&) t)...);
    }
  constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<Is...>) {
    return pushmi::invoke((F&&) f, std::get<Is>((Tuple&&) t)...);
  }
  template <class Tuple_, class Tuple = std::remove_reference_t<Tuple_>>
  using tupidxs = std::make_index_sequence<std::tuple_size<Tuple>::value>;
} // namespace detail

template <class F, class Tuple>
  requires requires (F&& f, Tuple&& t) {
    detail::apply_impl((F&&) f, (Tuple&&) t, detail::tupidxs<Tuple>{});
  }
constexpr decltype(auto) apply(F&& f, Tuple&& t) {
  return detail::apply_impl((F&&) f, (Tuple&&) t, detail::tupidxs<Tuple>{});
}
#endif

namespace detail {

template <class Tag>
struct make_receiver;
template <>
struct make_receiver<is_none<>> : construct_deduced<none> {};
template <>
struct make_receiver<is_single<>> : construct_deduced<single> {};

template <Sender In>
struct out_from_fn {
  using Cardinality = property_from_category_t<In, is_silent<>>;
  using Make = make_receiver<Cardinality>;
  template <class... Ts>
   requires Invocable<Make, Ts...>
  auto operator()(std::tuple<Ts...> args) const {
    return pushmi::apply(Make(), std::move(args));
  }
  template <class... Ts, class... Fns,
    class This = std::enable_if_t<sizeof...(Fns) != 0, out_from_fn>>
    requires And<SemiMovable<Fns>...> &&
      Invocable<Make, std::tuple<Ts...>> &&
      Invocable<This, pushmi::invoke_result_t<Make, std::tuple<Ts...>>, Fns...>
  auto operator()(std::tuple<Ts...> args, Fns...fns) const {
    return This()(This()(std::move(args)), std::move(fns)...);
  }
  template <Receiver<Cardinality> Out, class...Fns>
    requires And<SemiMovable<Fns>...>
  auto operator()(Out out, Fns... fns) const {
    return Make()(std::move(out), std::move(fns)...);
  }
};

PUSHMI_TEMPLATE(class In, class FN)
  (requires Sender<In> && SemiMovable<FN>)
auto submit_transform_out(FN fn){
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSender<In>) (
    return on_submit(
      constrain<mock::Receiver<_3>>(
        [fn = std::move(fn)](In& in, auto tp, auto out) {
          ::pushmi::submit(in, tp, fn(std::move(out)));
        }
      )
    );
  ) else (
    return on_submit(
      constrain<mock::Receiver<_2>>(
        [fn = std::move(fn)](In& in, auto out) {
          ::pushmi::submit(in, fn(std::move(out)));
        }
      )
    );
  ))
}

PUSHMI_TEMPLATE(class In, class SDSF, class TSDSF)
  (requires Sender<In> && SemiMovable<SDSF> && SemiMovable<TSDSF>)
auto submit_transform_out(SDSF sdsf, TSDSF tsdsf) {
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSender<In>) (
    return on_submit(
      constrain<mock::Receiver<_3>, mock::Invocable<TSDSF&, In&, _2, _3>>(
        [tsdsf = std::move(tsdsf)](In& in, auto tp, auto out) {
          tsdsf(in, tp, std::move(out));
        }
      )
    );
  ) else (
    return on_submit(
      constrain<mock::Receiver<_2>, mock::Invocable<SDSF&, In&, _2>>(
        [sdsf = std::move(sdsf)](In& in, auto out) {
          sdsf(in, std::move(out));
        }
      )
    );
  ))
}

template<Sender In, Receiver Out, class... FN>
auto deferred_from(FN&&... fn) {
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSenderTo<In, Out, is_single<>>) (
    return make_time_single_deferred((FN&&) fn...);
  ) else (
    PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out, is_single<>>) (
      return make_single_deferred((FN&&) fn...);
    ) else (
      PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out>) (
        return make_deferred((FN&&) fn...);
      ) else (
      ))
    ))
  ))
}

template<Sender In, Receiver Out, class... FN>
auto deferred_from(In in, FN&&... fn) {
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSenderTo<In, Out, is_single<>>) (
    return make_time_single_deferred(id(std::move(in)), (FN&&) fn...);
  ) else (
    PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out, is_single<>>) (
      return make_single_deferred(id(std::move(in)), (FN&&) fn...);
    ) else (
      PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out>) (
        return make_deferred(id(std::move(in)), (FN&&) fn...);
      ) else (
      ))
    ))
  ))
}

template<
    Sender In,
    Receiver Out,
    bool SenderRequires,
    bool SingleSenderRequires,
    bool TimeSingleSenderRequires>
constexpr bool deferred_requires_from() {
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSenderTo<In, Out, is_single<>>) (
    return TimeSingleSenderRequires;
  ) else (
    PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out, is_single<>>) (
      return SingleSenderRequires;
    ) else (
      PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out>) (
        return SenderRequires;
      ) else (
      ))
    ))
  ))
}

} // namespace detail

namespace extension_operators {

namespace detail{

struct set_value_fn {
  template<class V>
  auto operator()(V&& v) const {
    return constrain<mock::Receiver<_1, is_single<>>>(
        [v = (V&&) v](auto out) mutable {
          ::pushmi::set_value(out, (V&&) v);
        }
    );
  }
};

struct set_error_fn {
  PUSHMI_TEMPLATE(class E)
    (requires SemiMovable<E>)
  auto operator()(E e) const {
    return constrain<mock::NoneReceiver<_1, E>>(
      [e = std::move(e)](auto out) mutable {
        ::pushmi::set_error(out, std::move(e));
      }
    );
  }
};

struct set_done_fn {
  auto operator()() const {
    return constrain<mock::Receiver<_1>>(
      [](auto out) {
        ::pushmi::set_done(out);
      }
    );
  }
};

struct set_stopping_fn {
  auto operator()() const {
    return constrain<mock::Receiver<_1>>(
      [](auto out) {
        ::pushmi::set_stopping(out);
      }
    );
  }
};

struct set_starting_fn {
  template<Receiver Up>
  auto operator()(Up up) const {
    return constrain<mock::Receiver<_1>>(
      [up = std::move(up)](auto out) {
        ::pushmi::set_starting(out, std::move(up));
      }
    );
  }
};

struct submit_fn {
  template <Receiver Out>
  auto operator()(Out out) const {
    return constrain<mock::SenderTo<_1, Out>>(
      [out = std::move(out)](auto in) mutable {
        ::pushmi::submit(in, std::move(out));
      }
    );
  }
  template <class TP, Receiver Out>
  auto operator()(TP tp, Out out) const {
    return constrain<mock::TimeSenderTo<_1, Out>>(
      [tp = std::move(tp), out = std::move(out)](auto in) mutable {
        ::pushmi::submit(in, std::move(tp), std::move(out));
      }
    );
  }
};

struct now_fn {
  auto operator()() const {
    return constrain<mock::TimeSender<_1>>(
      [](auto in) {
        return ::pushmi::now(in);
      }
    );
  }
};

} // namespace detail

PUSHMI_INLINE_VAR constexpr detail::set_done_fn set_done{};
PUSHMI_INLINE_VAR constexpr detail::set_error_fn set_error{};
PUSHMI_INLINE_VAR constexpr detail::set_value_fn set_value{};
PUSHMI_INLINE_VAR constexpr detail::set_stopping_fn set_stopping{};
PUSHMI_INLINE_VAR constexpr detail::set_starting_fn set_starting{};
PUSHMI_INLINE_VAR constexpr detail::submit_fn submit{};
PUSHMI_INLINE_VAR constexpr detail::now_fn now{};
PUSHMI_INLINE_VAR constexpr detail::now_fn top{};

} // namespace extension_operators

} // namespace pushmi
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "../single.h"

namespace pushmi {

namespace operators {

PUSHMI_TEMPLATE(class V)
  (requires SemiMovable<V>)
auto just(V v) {
  return make_single_deferred(
    constrain<mock::SingleReceiver<_1, V>>(
      [v = std::move(v)](auto out) mutable {
        ::pushmi::set_value(out, std::move(v));
      }
    )
  );
}

} // namespace operators

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "../piping.h"
//#include "../executor.h"
//#include "extension_operators.h"

namespace pushmi {

namespace operators {

namespace detail {

struct on_fn {
  template <Invocable ExecutorFactory>
  auto operator()(ExecutorFactory ef) const;
};

template <Invocable ExecutorFactory>
auto on_fn::operator()(ExecutorFactory ef) const {
  return constrain<mock::Sender<_1>>([ef = std::move(ef)](auto in) {
    using In = decltype(in);
    return ::pushmi::detail::deferred_from<In, single<>>(
      std::move(in),
      ::pushmi::detail::submit_transform_out<In>(
        constrain<mock::SenderTo<In, _2>>([ef](In& in, auto out) {
          auto exec = ef();
          ::pushmi::submit(exec, ::pushmi::now(exec),
            ::pushmi::make_single([in = in, out = std::move(out)](auto) mutable {
              ::pushmi::submit(in, std::move(out));
            })
          );
        }),
        constrain<mock::TimeSenderTo<In, _3>>([ef](In& in, auto at, auto out) {
          auto exec = ef();
          ::pushmi::submit(exec, at,
            ::pushmi::on_value([in = in, at, out = std::move(out)](auto) mutable {
              ::pushmi::submit(in, at, std::move(out));
            })
          );
        })
      )
    );
  });
}

} // namespace detail

PUSHMI_INLINE_VAR constexpr detail::on_fn on{};

} // namespace operators

#if 0
namespace detail {

template <class ExecutorFactory>
class fsdon {
  using executor_factory_type = std::decay_t<ExecutorFactory>;

  executor_factory_type factory_;

  template <class In>
  class start_on {
    using in_type = std::decay_t<In>;

    executor_factory_type factory_;
    in_type in_;

    template <class Out, class Executor>
    class out_on {
      using out_type = std::decay_t<Out>;
      using exec_type = std::decay_t<Executor>;

      template <class Producer>
      struct producer_proxy {
        RefWrapper<Producer> up_;
        std::shared_ptr<std::atomic_bool> stopped_;
        exec_type exec_;

        producer_proxy(
            RefWrapper<Producer> p,
            std::shared_ptr<std::atomic_bool> stopped,
            exec_type exec)
            : up_(std::move(p)),
              stopped_(std::move(stopped)),
              exec_(std::move(exec)) {}

        template <class V>
        void value(V v) {
          auto up = wrap_ref(up_.get());
          exec_ |
              execute([up = std::move(up),
                       v = std::move(v),
                       stopped = std::move(stopped_)](auto) mutable {
                if (*stopped) {
                  return;
                }
                up.get().value(std::move(v));
              });
        }

        template <class E>
        void error(E e) {
          auto up = wrap_ref(up_.get());
          exec_ |
              execute([up = std::move(up),
                       e = std::move(e),
                       stopped = std::move(stopped_)](auto) mutable {
                if (*stopped) {
                  return;
                }
                up.get().error(std::move(e));
              });
        }
      };

      bool done_;
      std::shared_ptr<std::atomic_bool> stopped_;
      out_type out_;
      exec_type exec_;
      AnyNone<> upProxy_;

     public:
      out_on(out_type out, exec_type exec)
          : done_(false),
            stopped_(std::make_shared<std::atomic_bool>(false)),
            out_(std::move(out)),
            exec_(std::move(exec)),
            upProxy_() {}

      template <class T>
      void value(T t) {
        if (done_) {
          return;
        }
        done_ = true;
        out_.value(std::move(t));
      }

      template <class E>
      void error(E e) {
        if (done_) {
          return;
        }
        done_ = true;
        out_.error(std::move(e));
      }

      void stopping() {
        if (done_) {
          return;
        }
        done_ = true;
        *stopped_ = true;
        out_.stopping();
      }

      template <class Producer>
      void starting(RefWrapper<Producer> up) {
        upProxy_ =
            producer_proxy<Producer>{std::move(up), stopped_, std::move(exec_)};
        out_.starting(wrap_ref(upProxy_));
      }
    };

   public:
    start_on(executor_factory_type&& ef, in_type&& in)
        : factory_(std::move(ef)), in_(std::move(in)) {}

    template <class Out>
    auto then(Out out) {
      auto exec = factory_();
      auto myout = out_on<Out, decltype(exec)>{std::move(out), exec};
      exec | execute([in = in_, myout = std::move(myout)](auto) mutable {
        in.then(std::move(myout));
      });
    }
  };

 public:
  explicit fsdon(executor_factory_type&& ef) : factory_(std::move(ef)) {}

  template <class In>
  auto operator()(In in) {
    return start_on<In>{std::move(factory_), std::move(in)};
  }
};

} // namespace detail

namespace fsd {
template <class ExecutorFactory>
auto on(ExecutorFactory factory) {
  return detail::fsdon<ExecutorFactory>{std::move(factory)};
}
} // namespace fsd
#endif

} // namespace pushmi
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <functional>
//#include "../single.h"
//#include "../boosters.h"
//#include "extension_operators.h"
//#include "../trampoline.h"
//#include "../detail/opt.h"
//#include "../detail/if_constexpr.h"

namespace pushmi {
namespace operators {
namespace detail {

template <Sender In, class ...AN>
using receiver_type_t =
    pushmi::invoke_result_t<
        pushmi::detail::make_receiver<property_from_category_t<In, is_silent<>>>,
        AN...>;

template <class In, class ... AN>
concept bool AutoSenderTo = SenderTo<In, receiver_type_t<In, AN...>>;

template <class In, class ... AN>
concept bool AutoTimeSenderTo = TimeSenderTo<In, receiver_type_t<In, AN...>>;

struct submit_fn {
private:
  // TODO - only move, move-only types..
  // if out can be copied, then submit can be called multiple
  // times..
  template <class... AN>
  struct fn {
    std::tuple<AN...> args_;
    template <AutoSenderTo<AN...> In>
    In operator()(In in) {
      auto out{::pushmi::detail::out_from_fn<In>()(std::move(args_))};
      ::pushmi::submit(in, std::move(out));
      return in;
    }
    template <AutoTimeSenderTo<AN...> In>
    In operator()(In in) {
      auto out{::pushmi::detail::out_from_fn<In>()(std::move(args_))};
      ::pushmi::submit(in, ::pushmi::now(in), std::move(out));
      return in;
    }
  };
public:
  template <class... AN>
  auto operator()(AN&&... an) const {
    return submit_fn::fn<AN...>{{(AN&&) an...}};
  }
};

struct submit_at_fn {
private:
  template <class TP, class...AN>
  struct fn {
    TP at_;
    std::tuple<AN...> args_;
    template <AutoTimeSenderTo<AN...> In>
    In operator()(In in) {
      auto out{::pushmi::detail::out_from_fn<In>()(std::move(args_))};
      ::pushmi::submit(in, std::move(at_), std::move(out));
      return in;
    }
  };
public:
  PUSHMI_TEMPLATE(class TP, class...AN)
    (requires Regular<TP>)
  auto operator()(TP at, AN... an) const {
    return submit_at_fn::fn<TP, AN...>{std::move(at), {(AN&&) an...}};
  }
};

struct submit_after_fn {
private:
  template <class D, class... AN>
  struct fn {
    D after_;
    std::tuple<AN...> args_;
    template <AutoTimeSenderTo<AN...> In>
    In operator()(In in) {
      // TODO - only move, move-only types..
      // if out can be copied, then submit can be called multiple
      // times..
      auto out{::pushmi::detail::out_from_fn<In>()(std::move(args_))};
      auto at = ::pushmi::now(in) + std::move(after_);
      ::pushmi::submit(in, std::move(at), std::move(out));
      return in;
    }
  };
public:
  PUSHMI_TEMPLATE(class D, class...AN)
    (requires Regular<D>)
  auto operator()(D after, AN... an) const {
    return submit_after_fn::fn<D, AN...>{std::move(after), {(AN&&) an...}};
  }
};

struct blocking_submit_fn {
private:
  // TODO - only move, move-only types..
  // if out can be copied, then submit can be called multiple
  // times..
  template <class... AN>
  struct fn {
    std::tuple<AN...> args_;

    template <bool IsTimeSender, class In>
    In impl_(In in) {
      bool done = false;
      std::condition_variable signaled;
      auto out{::pushmi::detail::out_from_fn<In>()(
        std::move(args_),
        on_value(constrain<mock::Receiver<_1, is_single<>>>(
          [&](auto out, auto&& v) {
            using V = remove_cvref_t<decltype(v)>;
            PUSHMI_IF_CONSTEXPR( ((bool)Time<V>) (
              // to keep the blocking semantics, make sure that the
              // nested submits block here to prevent a spurious
              // completion signal
              auto nest = ::pushmi::nested_trampoline();
              ::pushmi::submit(nest, ::pushmi::now(nest), std::move(out));
            ) else (
              ::pushmi::set_value(out, id((V&&) v));
            ))
            done = true;
            signaled.notify_all();
          }
        )),
        on_error(constrain<mock::NoneReceiver<_1, _2>>(
          [&](auto out, auto e) noexcept {
            ::pushmi::set_error(out, std::move(e));
            done = true;
            signaled.notify_all();
          }
        )),
        on_done(constrain<mock::Receiver<_1>>(
          [&](auto out){
            ::pushmi::set_done(out);
            done = true;
            signaled.notify_all();
          }
        ))
      )};
      PUSHMI_IF_CONSTEXPR( (IsTimeSender) (
        id(::pushmi::submit)(in, id(::pushmi::now)(in), std::move(out));
      ) else (
        id(::pushmi::submit)(in, std::move(out));
      ))
      std::mutex lock;
      std::unique_lock<std::mutex> guard{lock};
      signaled.wait(guard, [&](){
        return done;
      });
      return in;
    }

    template <AutoSenderTo<AN...> In>
    In operator()(In in) {
      return this->impl_<false>(std::move(in));
    }
    template <AutoTimeSenderTo<AN...> In>
    In operator()(In in) {
      return this->impl_<true>(std::move(in));
    }
  };
public:
  template <class... AN>
  auto operator()(AN... an) const {
    return blocking_submit_fn::fn<AN...>{{(AN&&) an...}};
  }
};

template <class T>
struct get_fn {
  // TODO constrain this better
  template <Sender In>
  T operator()(In in) const {
    pushmi::detail::opt<T> result_;
    std::exception_ptr ep_;
    auto out = make_single(
      on_value([&](T t){ result_ = std::move(t); }),
      on_error(
        [&](auto e) noexcept { ep_ = std::make_exception_ptr(e); },
        [&](std::exception_ptr ep) noexcept { ep_ = ep; })
    );
    using Out = decltype(out);
    static_assert(SenderTo<In, Out, is_single<>> ||
        TimeSenderTo<In, Out, is_single<>>,
        "'In' does not deliver value compatible with 'T' to 'Out'");
    blocking_submit_fn{}(std::move(out))(in);
    if (!!ep_) { std::rethrow_exception(ep_); }
    return std::move(*result_);
  }
};

} // namespace detail

PUSHMI_INLINE_VAR constexpr detail::submit_fn submit{};
PUSHMI_INLINE_VAR constexpr detail::submit_at_fn submit_at{};
PUSHMI_INLINE_VAR constexpr detail::submit_after_fn submit_after{};
PUSHMI_INLINE_VAR constexpr detail::blocking_submit_fn blocking_submit{};
template <class T>
PUSHMI_INLINE_VAR constexpr detail::get_fn<T> get{};

} // namespace operators

} // namespace pushmi
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include <cassert>
//#include "extension_operators.h"
//#include "../deferred.h"
//#include "../single_deferred.h"
//#include "../time_single_deferred.h"

namespace pushmi {

namespace operators {
namespace detail {
PUSHMI_INLINE_VAR constexpr struct as_const_fn {
  template <class T>
  constexpr const T& operator()(T& t) const noexcept {
    return t;
  }
} const as_const {};

template <Receiver SideEffects, Receiver Out>
struct tap_ {
  SideEffects sideEffects;
  Out out;

  // side effect has no effect on the properties.
  using properties = properties_t<Out>;

  template <class V, class UV = std::remove_reference_t<V>>
    requires 
    // SingleReceiver<SideEffects, const UV&> && 
    SingleReceiver<Out, V>
  void value(V&& v) {
    ::pushmi::set_value(sideEffects, as_const(v));
    ::pushmi::set_value(out, (V&&) v);
  }
  template <class E>
    requires 
    // NoneReceiver<SideEffects, const E&> && 
    NoneReceiver<Out, E>
  void error(E e) noexcept {
    ::pushmi::set_error(sideEffects, as_const(e));
    ::pushmi::set_error(out, std::move(e));
  }
  void done() {
    ::pushmi::set_done(sideEffects);
    ::pushmi::set_done(out);
  }
};

template <Receiver SideEffects, Receiver Out>
  requires Receiver<tap_<SideEffects, Out>, property_from_category_t<Out, is_silent<>>>
auto make_tap(SideEffects se, Out out) -> tap_<SideEffects, Out> {
  return {std::move(se), std::move(out)};
}

struct tap_fn {
  template <class... AN>
  auto operator()(AN... an) const;
};

#if __cpp_if_constexpr >= 201606
#define PUSHMI_STATIC_ASSERT static_assert
#else
#define PUSHMI_STATIC_ASSERT detail::do_assert
inline void do_assert(bool condition, char const*) {
  assert(condition);
}
#endif

template <class... AN>
auto tap_fn::operator()(AN... an) const {
  return constrain<mock::Sender<_1>>(
    [args = std::tuple<AN...>{std::move(an)...}](auto in) mutable {
      using In = decltype(in);
      auto sideEffects{::pushmi::detail::out_from_fn<In>()(std::move(args))};
      using SideEffects = decltype(sideEffects);

      PUSHMI_STATIC_ASSERT(
        ::pushmi::detail::deferred_requires_from<In, SideEffects,
          SenderTo<In, SideEffects, is_none<>>,
          SenderTo<In, SideEffects, is_single<>>,
          TimeSenderTo<In, SideEffects, is_single<>> >(),
          "'In' is not deliverable to 'SideEffects'");

      return ::pushmi::detail::deferred_from<In, SideEffects>(
        std::move(in),
        ::pushmi::detail::submit_transform_out<In>(
          constrain<mock::Receiver<_1>>(
            [sideEffects = std::move(sideEffects)](auto out) {
              using Out = decltype(out);
              PUSHMI_STATIC_ASSERT(
                ::pushmi::detail::deferred_requires_from<In, SideEffects,
                  SenderTo<In, Out, is_none<>>,
                  SenderTo<In, Out, is_single<>>,
                  TimeSenderTo<In, Out, is_single<>> >(),
                  "'In' is not deliverable to 'Out'");
              auto gang{::pushmi::detail::out_from_fn<In>()(
                  detail::make_tap(sideEffects, std::move(out)))};
              using Gang = decltype(gang);
              PUSHMI_STATIC_ASSERT(
                ::pushmi::detail::deferred_requires_from<In, SideEffects,
                  SenderTo<In, Gang>,
                  SenderTo<In, Gang, is_single<>>,
                  TimeSenderTo<In, Gang, is_single<>> >(),
                  "'In' is not deliverable to 'Out' & 'SideEffects'");
              return gang;
            }
          )
        )
      );
    }
  );
}

} // namespace detail

PUSHMI_INLINE_VAR constexpr detail::tap_fn tap{};

} // namespace operators

} // namespace pushmi
// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "../single.h"
//#include "submit.h"
//#include "extension_operators.h"

namespace pushmi {

namespace operators {

namespace detail {

struct transform_fn {
  template <class... FN>
  auto operator()(FN... fn) const;
};

template <class... FN>
auto transform_fn::operator()(FN... fn) const {
  auto f = overload(std::move(fn)...);
  return constrain<mock::Sender<_1>>([f = std::move(f)](auto in) {
    using In = decltype(in);
    // copy 'f' to allow multiple calls to connect to multiple 'in'
    return ::pushmi::detail::deferred_from<In, single<>>(
      std::move(in),
      ::pushmi::detail::submit_transform_out<In>(
        constrain<mock::Receiver<_1>>([f](auto out) {
          using Out = decltype(out);
          return ::pushmi::detail::out_from_fn<In>()(
            std::move(out),
            // copy 'f' to allow multiple calls to submit
            on_value(
              [f](Out& out, auto&& v) {
                using V = decltype(v);
                using Result = decltype(f((V&&) v));
                static_assert(SemiMovable<Result>,
                  "none of the functions supplied to transform can convert this value");
                static_assert(SingleReceiver<Out, Result>,
                  "Result of value transform cannot be delivered to Out");
                ::pushmi::set_value(out, f((V&&) v));
              }
            )
          );
        })
      )
    );
  });
}

} // namespace detail

PUSHMI_INLINE_VAR constexpr detail::transform_fn transform{};

} // namespace operators

} // namespace pushmi
//#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

//#include "../piping.h"
//#include "../executor.h"
//#include "extension_operators.h"

namespace pushmi {

namespace operators {

namespace detail {

class via_fn {
  template <Invocable ExecutorFactory>
  auto operator()(ExecutorFactory ef) const;
};

template<class Executor, class Out>
struct via_fn_data : public Out {
  Executor exec;

  via_fn_data(Out out, Executor exec) :
    Out(std::move(out)), exec(std::move(exec)) {}
};

template<class Out, class Executor>
auto make_via_fn_data(Out out, Executor ex) -> via_fn_data<Executor, Out> {
  return via_fn_data<Executor, Out>{std::move(out), std::move(ex)};
}

template <Invocable ExecutorFactory>
auto via_fn::operator()(ExecutorFactory ef) const {
  return constrain<mock::Sender<_1>>([ef = std::move(ef)](auto in) {
    using In = decltype(in);
    return ::pushmi::detail::deferred_from<In, single<>>(
      std::move(in),
      ::pushmi::detail::submit_transform_out<In>(
        constrain<mock::Receiver<_1>>([ef](auto out) {
          using Out = decltype(out);
          auto exec = ef();
          return ::pushmi::detail::out_from_fn<In>()(
            make_via_fn_data(std::move(out), std::move(exec)),
            // copy 'f' to allow multiple calls to submit
            ::pushmi::on_value([](auto& data, auto&& v) {
              using V = decltype(v);
              ::pushmi::submit(
                data.exec,
                ::pushmi::now(data.exec),
                ::pushmi::make_single(
                  [v = (V&&)v, out = std::move(static_cast<Out&>(data))](auto) mutable {
                    ::pushmi::set_value(out, std::move(v));
                  }
                )
              );
            }),
            ::pushmi::on_error([](auto& data, auto e) noexcept {
              ::pushmi::submit(
                data.exec,
                ::pushmi::now(data.exec),
                ::pushmi::make_single(
                  [e = std::move(e), out = std::move(static_cast<Out&>(data))](auto) mutable {
                    ::pushmi::set_error(out, std::move(e));
                  }
                )
              );
            }),
            ::pushmi::on_done([](auto& data){
              ::pushmi::submit(
                data.exec,
                ::pushmi::now(data.exec),
                ::pushmi::make_single(
                  [out = std::move(static_cast<Out&>(data))](auto) mutable {
                    ::pushmi::set_done(out);
                  }
                )
              );
            })
          );
        })
      )
    );
  });
}

} // namespace detail

PUSHMI_INLINE_VAR constexpr detail::via_fn via{};

} // namespace operators

#if 0

namespace detail {

template <class ExecutorFactory>
class fsdvia {
  using executor_factory_type = std::decay_t<ExecutorFactory>;

  executor_factory_type factory_;

  template <class In>
  class start_via {
    using in_type = std::decay_t<In>;

    executor_factory_type factory_;
    in_type in_;

    template <class Out, class Executor>
    class out_via {
      using out_type = std::decay_t<Out>;
      using executor_type = std::decay_t<Executor>;

      struct shared_type {
        shared_type(out_type&& out) : out_(std::move(out)), stopped_(false) {}
        out_type out_;
        std::atomic_bool stopped_;
      };

      template <class Producer>
      struct producer_proxy {
        RefWrapper<Producer> up_;
        std::shared_ptr<shared_type> shared_;

        producer_proxy(RefWrapper<Producer> p, std::shared_ptr<shared_type> s)
            : up_(std::move(p)), shared_(std::move(s)) {}

        template <class V>
        void value(V v) {
          if (!!shared_->stopped_.exchange(true)) {
            return;
          }
          up_.get().value(std::move(v));
        }

        template <class E>
        void error(E e) {
          if (!!shared_->stopped_.exchange(true)) {
            return;
          }
          up_.get().error(std::move(e));
        }
      };

      bool done_;
      std::shared_ptr<shared_type> shared_;
      executor_type exec_;
      std::shared_ptr<AnyNone<>> upProxy_;

     public:
      explicit out_via(out_type&& out, executor_type&& exec)
          : done_(false),
            shared_(std::make_shared<shared_type>(std::move(out))),
            exec_(std::move(exec)),
            upProxy_() {}

      template <class T>
      void value(T t) {
        if (done_ || shared_->stopped_) {
          done_ = true;
          return;
        }
        if (!upProxy_) {
          std::abort();
        }
        done_ = true;
        exec_ | execute([t = std::move(t), shared = shared_](auto) mutable {
          shared->out_.value(std::move(t));
        });
      }

      template <class E>
      void error(E e) {
        if (done_ || shared_->stopped_) {
          done_ = true;
          return;
        }
        if (!upProxy_) {
          std::abort();
        }
        done_ = true;
        exec_ | execute([e = std::move(e), shared = shared_](auto) mutable {
          shared->out_.error(std::move(e));
        });
      }

      void stopping() {
        if (done_) {
          return;
        }
        if (!upProxy_) {
          std::abort();
        }
        done_ = true;
        if (!shared_->stopped_.exchange(true)) {
          exec_ |
              // must keep out and upProxy alive until out is notified that it
              // is unsafe
              execute([shared = shared_](auto) mutable {
                shared->out_.stopping();
              });
        }
      }

      template <class Producer>
      void starting(RefWrapper<Producer> up) {
        if (!!upProxy_) {
          std::abort();
        }
        upProxy_ = std::make_shared<AnyNone<>>(AnyNone<>{
            producer_proxy<Producer>{std::move(up), shared_}});
        // must keep out and upProxy alive until out is notified that it is
        // starting
        exec_ | execute([shared = shared_, upProxy = upProxy_](auto) mutable {
          shared->out_.starting(wrap_ref(*upProxy));
        });
      }
    };

   public:
    start_via(executor_factory_type&& ef, in_type&& in)
        : factory_(ef), in_(in) {}

    template <class Out>
    auto then(Out out) {
      auto exec = factory_();
      in_.then(out_via<Out, decltype(exec)>{std::move(out), std::move(exec)});
    }
  };

 public:
  explicit fsdvia(executor_factory_type&& ef) : factory_(std::move(ef)) {}

  template <class In>
  auto operator()(In in) {
    return start_via<In>{std::move(factory_), std::move(in)};
  }
};

} // namespace detail

namespace fsd {

template <class ExecutorFactory>
auto via(ExecutorFactory factory) {
  return detail::fsdvia<ExecutorFactory>{std::move(factory)};
}

} // namespace fsd
#endif

} // namespace pushmi

#endif // PUSHMI_SINGLE_HEADER