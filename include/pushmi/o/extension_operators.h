// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <tuple>
#include "../piping.h"
#include "../boosters.h"
#include "../single.h"
#include "../detail/if_constexpr.h"
#include "../detail/functional.h"

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
struct make_receiver<none_tag> : construct_deduced<none> {};
template <>
struct make_receiver<single_tag> : construct_deduced<single> {};

template <Sender In>
struct out_from_fn {
  using Make = make_receiver<sender_category_t<In>>;
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
  template <Receiver<sender_category_t<In>> Out, class...Fns>
    requires And<SemiMovable<Fns>...>
  auto operator()(Out out, Fns... fns) const {
    return Make()(std::move(out), std::move(fns)...);
  }
};

template<Sender In, SemiMovable FN>
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

template<Sender In, SemiMovable SDSF, SemiMovable TSDSF>
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
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSenderTo<In, Out, single_tag>) (
    return make_time_single_deferred((FN&&) fn...);
  ) else (
    PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out, single_tag>) (
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
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSenderTo<In, Out, single_tag>) (
    return make_time_single_deferred(id(std::move(in)), (FN&&) fn...);
  ) else (
    PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out, single_tag>) (
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
  PUSHMI_IF_CONSTEXPR_RETURN( ((bool)TimeSenderTo<In, Out, single_tag>) (
    return TimeSingleSenderRequires;
  ) else (
    PUSHMI_IF_CONSTEXPR_RETURN( ((bool)SenderTo<In, Out, single_tag>) (
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
    return constrain<mock::Receiver<_1, single_tag>>(
        [v = (V&&) v](auto out) mutable {
          ::pushmi::set_value(out, (V&&) v);
        }
    );
  }
};

struct set_error_fn {
  template<SemiMovable E>
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
