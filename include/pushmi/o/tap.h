// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <cassert>
#include "extension_operators.h"
#include "../deferred.h"
#include "../single_deferred.h"
#include "../time_single_deferred.h"

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

  using side_effects_tag = receiver_category_t<SideEffects>;
  using out_tag = receiver_category_t<Out>;

  using receiver_category = std::common_type_t<side_effects_tag, out_tag>;

  template <class V, class UV = std::remove_reference_t<V>>
    requires SingleReceiver<SideEffects, const UV&> && SingleReceiver<Out, V>
  void value(V&& v) {
    ::pushmi::set_value(sideEffects, as_const(v));
    ::pushmi::set_value(out, (V&&) v);
  }
  template <class E>
    requires NoneReceiver<SideEffects, const E&> && NoneReceiver<Out, E>
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
          SenderTo<In, SideEffects, none_tag>,
          SenderTo<In, SideEffects, single_tag>,
          TimeSenderTo<In, SideEffects, single_tag> >(),
          "'In' is not deliverable to 'SideEffects'");

      return ::pushmi::detail::deferred_from<In, SideEffects>(
        std::move(in),
        ::pushmi::detail::submit_transform_out<In>(
          constrain<mock::Receiver<_1>>(
            [sideEffects = std::move(sideEffects)](auto out) {
              using Out = decltype(out);
              PUSHMI_STATIC_ASSERT(
                ::pushmi::detail::deferred_requires_from<In, SideEffects,
                  SenderTo<In, Out, none_tag>,
                  SenderTo<In, Out, single_tag>,
                  TimeSenderTo<In, Out, single_tag> >(),
                  "'In' is not deliverable to 'Out'");
              auto gang{::pushmi::detail::out_from_fn<In>()(
                  detail::make_tap(sideEffects, std::move(out)))};
              using Gang = decltype(gang);
              PUSHMI_STATIC_ASSERT(
                ::pushmi::detail::deferred_requires_from<In, SideEffects,
                  SenderTo<In, Gang>,
                  SenderTo<In, Gang, single_tag>,
                  TimeSenderTo<In, Gang, single_tag> >(),
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