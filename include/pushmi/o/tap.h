// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <cassert>
#include "extension_operators.h"
#include "../sender.h"
#include "../single_sender.h"
#include "../time_single_sender.h"

namespace pushmi {

namespace detail {

PUSHMI_TEMPLATE(class SideEffects, class Out)
  (requires Receiver<SideEffects> && Receiver<Out>)
struct tap_ {
  SideEffects sideEffects;
  Out out;

  // side effect has no effect on the properties.
  using properties = properties_t<Out>;

  PUSHMI_TEMPLATE(class... VN)
    (requires
      ReceiveValue<SideEffects, const std::remove_reference_t<VN>...> &&
      ReceiveValue<Out, std::remove_reference_t<VN>...>)
  void value(VN&&... vn) {
    ::pushmi::set_value(sideEffects, as_const(vn)...);
    ::pushmi::set_value(out, (VN&&) vn...);
  }
  PUSHMI_TEMPLATE(class E)
    (requires
      ReceiveError<SideEffects, const E> &&
      ReceiveError<Out, E>)
  void error(E e) noexcept {
    ::pushmi::set_error(sideEffects, as_const(e));
    ::pushmi::set_error(out, std::move(e));
  }
  void done() {
    ::pushmi::set_done(sideEffects);
    ::pushmi::set_done(out);
  }
  PUSHMI_TEMPLATE(class Up, class UUp = std::remove_reference_t<Up>)
    (requires
      FlowReceiver<SideEffects> &&
      FlowReceiver<Out>)
  void starting(Up&& up) {
    // up is not made const because sideEffects is allowed to call methods on up
    ::pushmi::set_starting(sideEffects, up);
    ::pushmi::set_starting(out, (Up&&) up);
  }
};

PUSHMI_INLINE_VAR constexpr struct make_tap_fn {
  PUSHMI_TEMPLATE(class SideEffects, class Out)
    (requires Receiver<SideEffects> && Receiver<Out> &&
      Receiver<tap_<SideEffects, Out>>)
  auto operator()(SideEffects se, Out out) const {
    return tap_<SideEffects, Out>{std::move(se), std::move(out)};
  }
} const make_tap {};

struct tap_fn {
private:
  PUSHMI_TEMPLATE (class In, class SideEffects)
    (requires Sender<In> && Receiver<SideEffects>)
  static auto impl(In in, SideEffects sideEffects) {
    return ::pushmi::detail::sender_from(
      std::move(in),
      ::pushmi::detail::submit_transform_out<In>(
        out_impl<In, SideEffects>{std::move(sideEffects)}
      )
    );
  }

  template <class... AN>
  struct in_impl {
    std::tuple<AN...> args_;
    PUSHMI_TEMPLATE (class In)
      (requires Sender<In>)
    auto operator()(In in) {
      return tap_fn::impl(
        std::move(in),
        ::pushmi::detail::receiver_from_fn<In>()(std::move(args_)));
    }
  };
  PUSHMI_TEMPLATE (class In, class SideEffects)
    (requires Sender<In> && Receiver<SideEffects>)
  struct out_impl {
    SideEffects sideEffects_;
    PUSHMI_TEMPLATE (class Out)
      (requires Receiver<Out> && SenderTo<In, Out> &&
        SenderTo<In, decltype(::pushmi::detail::receiver_from_fn<In>()(
            detail::make_tap(std::declval<SideEffects>(), std::declval<Out>())))>)
    auto operator()(Out out) const {
      auto gang{::pushmi::detail::receiver_from_fn<In>()(
          detail::make_tap(sideEffects_, std::move(out)))};
      return gang;
    }
  };
public:
  template <class... AN>
  auto operator()(AN... an) const  {
    return in_impl<AN...>{{std::move(an)...}};
  }
};

} // namespace detail

namespace operators {
PUSHMI_INLINE_VAR constexpr detail::tap_fn tap{};
} // namespace operators

} // namespace pushmi
