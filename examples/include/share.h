// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <pushmi/single.h>
#include <pushmi/o/submit.h>
#include <pushmi/o/extension_operators.h>

#include <subject.h>

namespace pushmi {

namespace operators {

namespace detail {

template<class T>
struct share_fn {
  auto operator()() const;
};


template<class T>
auto share_fn<T>::operator()() const {
  return constrain(lazy::Sender<_1>, [](auto in) {
    using In = decltype(in);
    subject<T, properties_t<In>> sub;

    PUSHMI_IF_CONSTEXPR( ((bool)TimeSender<In>) (
      ::pushmi::submit(in, ::pushmi::now(id(in)), sub.receiver());
    ) else (
      ::pushmi::submit(in, sub.receiver());
    ));

    return sub;
  });
}

} // namespace detail

template<class T>
PUSHMI_INLINE_VAR constexpr detail::share_fn<T> share{}; 

} // namespace operators

} // namespace pushmi
