#pragma once

// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <experimental/thread_pool>

#include <pushmi/executor.h>
#include <pushmi/trampoline.h>

namespace pushmi {

using std::experimental::static_thread_pool;
namespace execution = std::experimental::execution;

template<class Executor>
struct __pool_submit {
  using e_t = Executor;
  e_t e;
  explicit __pool_submit(e_t e) : e(std::move(e)) {}
  template<Regular TP, Receiver Out>
  void operator()(TP at, Out out) const;
};

template<class Executor>
template<Regular TP, Receiver Out>
void __pool_submit<Executor>::operator()(TP at, Out out) const {
  e.execute([at = std::move(at), out = std::move(out)]() mutable {
    auto tr = trampoline();
    ::pushmi::submit(tr, std::move(at), std::move(out));
  });
}

class pool {
  static_thread_pool p;
public:

  inline explicit pool(std::size_t threads) : p(threads) {}

  inline auto executor() {
    auto exec = execution::require(p.executor(), execution::never_blocking, execution::oneway);
    return make_time_single_deferred(__pool_submit<decltype(exec)>{exec});
  }

  inline void stop() {p.stop();}
  inline void wait() {p.wait();}
};

}
