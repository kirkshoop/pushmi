// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "../sender.h"
#include "../single_sender.h"
#include "../detail/functional.h"

namespace pushmi {
namespace detail {
  template <class... VN>
  struct single_empty_impl {
    PUSHMI_TEMPLATE(class Out)
      (requires ReceiveValue<Out, VN...>)
    void operator()(Out out) {
      ::pushmi::set_done(out);
    }
  };
  struct empty_impl {
    PUSHMI_TEMPLATE(class Out)
      (requires ReceiveValue<Out>)
    void operator()(Out out) {
      ::pushmi::set_done(out);
    }
  };
}

namespace operators {
template <class... VN>
auto empty() {
  return make_single_sender(detail::single_empty_impl<VN...>{});
}

inline auto empty() {
  return make_sender(detail::empty_impl{});
}

} // namespace operators
} // namespace pushmi
