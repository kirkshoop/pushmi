#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "single.h"

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
  using sender_category = single_tag;

  time_single_deferred() = default;
  time_single_deferred(time_single_deferred&& that) noexcept
      : time_single_deferred() {
    that.vptr_->op_(that.data_, &data_);
    std::swap(that.vptr_, vptr_);
  }
  template <class Wrapped, Sender<single_tag> W = wrapped_t<Wrapped>>
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
  template <class Wrapped, Sender<single_tag> W = wrapped_t<Wrapped>>
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
  using sender_category = single_tag;

  constexpr time_single_deferred() = default;
  constexpr explicit time_single_deferred(SF sf)
      : sf_(std::move(sf)) {}
  constexpr time_single_deferred(SF sf, NF nf)
      : sf_(std::move(sf)), nf_(std::move(nf)) {}
  auto now() {
    return nf_();
  }
  template <Regular TP, Receiver<single_tag> Out>
    requires Invocable<SF&, TP, Out>
  void submit(TP tp, Out out) {
    sf_(std::move(tp), std::move(out));
  }
};

template <TimeSender<single_tag> Data, class DSF, Invocable<Data&> DNF>
class time_single_deferred<Data, DSF, DNF> {
  Data data_{};
  DSF sf_{};
  DNF nf_{};

 public:
  using sender_category = single_tag;

  constexpr time_single_deferred() = default;
  constexpr explicit time_single_deferred(Data data)
      : data_(std::move(data)) {}
  constexpr time_single_deferred(Data data, DSF sf, DNF nf = DNF{})
      : data_(std::move(data)), sf_(std::move(sf)), nf_(std::move(nf)) {}
  auto now() {
    return nf_(data_);
  }

  template <class TP, Receiver<single_tag> Out>
    requires Invocable<DSF&, Data&, TP, Out>
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
template <TimeSender<single_tag> Data, class DSF>
auto make_time_single_deferred(Data d, DSF sf) ->
    time_single_deferred<Data, DSF, passDNF> {
  return {std::move(d), std::move(sf)};
}
template <TimeSender<single_tag> Data, class DSF, class DNF>
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

template <TimeSender<single_tag> Data, class DSF>
time_single_deferred(Data, DSF) -> time_single_deferred<Data, DSF, passDNF>;

template <TimeSender<single_tag> Data, class DSF, class DNF>
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
//     TimeSenderTo<single<V, E>, single_tag> Wrapped>
// auto erase_cast(Wrapped w) {
//   return time_single_deferred<V, E>{std::move(w)};
// }

} // namespace pushmi