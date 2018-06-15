#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "boosters.h"

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
