#pragma once

// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <bulk.h>
#include <pushmi/o/submit.h>
#include <pushmi/o/just.h>

namespace pushmi {

template<class ExecutionPolicy, class RandomAccessIterator, class Function>
void for_each(ExecutionPolicy&& policy, RandomAccessIterator first, RandomAccessIterator last, Function f)
{
  auto n = last - first;

  future_from<int>(operators::just(std::make_tuple(first, f)) | 
    operators::bulk(
      [](auto& acc, auto idx){ return std::get<1>(acc)(std::get<0>(acc)[idx]); }, 
      n, 
      policy, 
      [](auto&& args){ return args; }, 
      [](auto&& acc){ return 0; })).wait();
}

} // namespace pushmi
