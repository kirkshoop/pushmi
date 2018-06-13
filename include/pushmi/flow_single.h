#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "single.h"

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
  using receiver_category = flow_tag;

  flow_single() = default;
  flow_single(flow_single&& that) noexcept : flow_single() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped>
    requires FlowSingle<wrapped_t<Wrapped>, any_none<PE>, V, PE, E>
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
    requires FlowSingle<wrapped_t<Wrapped>, any_none<PE>, V, PE, E> && insitu<Wrapped>()
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
  using receiver_category = flow_tag;

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
  template <Receiver<none_tag> Up>
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
  using receiver_category = flow_tag;

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
    requires FlowSingle<Wrapped, V, PE, E> &&
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
    requires FlowSingle<Wrapped, V, PE, E> &&
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
//     requires FlowSingle<Wrapped, V, PE, E> && !detail::is_v<Wrapped, none> &&
//     !detail::is_v<Wrapped, std::promise>
//     auto erase_cast(Wrapped w) {
//   return flow_single<V, PE, E>{std::move(w)};
// }

} // namespace pushmi
