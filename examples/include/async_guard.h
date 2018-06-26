// clang-format off
// clang format does not support the '<>' in the lambda syntax yet.. []<>()->{}
#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <pushmi/single.h>

namespace pushmi {

namespace detail {

// a library that allows a guard to be created that collects 
// all the completion and cleanup signals and blocks until 
// all the registered work was completed.

struct async_guard_stack {
  // FIXME - need a way to reap completed scopes to prevent growing heap in long running processes.
  struct scope_t {
      std::function<void()> w;
      void wait() {w();}
  };
  struct guard_t {

      explicit guard_t(std::shared_ptr<guard_t> r) : root(std::move(r)) {}

      using scopes_t = std::vector<scope_t>;
      std::shared_ptr<guard_t> root;
      scopes_t local;
      std::mutex lock;
      scopes_t concurrent;

      void async_local(scope_t s) {
          local.push_back(s);
      }

      void async_concurrent(scopes_t s) {
          std::unique_lock<std::mutex> guard{lock};
          concurrent.append(s.begin(), s.end());
      }

      void async_wait(scopes_t s) {
        if (!!root) { std::abort(); }

        for (;;) {
          bool exit = false;
          concurrent_scopes_t expired;
          {
            std::unique_lock<std::mutex> guard{lock};
            expired = std::move(concurrent);
            // FIXME need to set exit to true when all concurrent guards are out-of-scope
          }
          for (auto& s : expired){
            s.wait();
          }
          if (exit) {break;}
        }
      }

      void wait() {
        if (!root) {
          // this is the root
          async_concurrent(local);
          async_wait();
        } else {
          // delegate to the root
          root->async_concurrent(local);
        }
      }
  };

  using stack_t = std::stack<std::shared_ptr<guard_t>>;

  inline static stack_t& stack() {
    static thread_local stack_t s{};
    return s;
  }

  inline static const std::shared_ptr<guard_t>& top() {
    return stack().top();
  }

  inline static void pop() {
    stack().pop();
  }

  inline static void push(std::shared_ptr<guard_t> root) {
    stack().push(std::make_shared<guard_t>(root));
  }
};

} // namespace detail 

// use async_guard to wrap F with a safe async scope that does not block in the destructor.
//
// in main and other scopes that must block, use async_guard without the root param to 
// create a new root scope.
//
// in cases that should not block (executors etc..) use async_delegate() to capture the 
// root scope during construction and pass the stored root to an async_guard around 
// each continuation.
PUSHMI_TEMPLATE(class F)
  (requires Invocable<F>)
void aync_guard(F f, std::shared_ptr<detail::async_guard_stack::guard_t> root = nullptr) {
  try {
    detail::async_guard_stack::push(root);
    f();
    auto g = std::move(detail::async_guard_stack::top());
    detail::async_guard_stack::pop();
    g->wait();
  } catch (...) {
    // unhandled exceptions inside an async scope 
    // require the process to exit.
    std::abort();
  }
}

// FIXME use a new return type here that tracks lifetime by ref count and use this 
// refcount to FIXME the exit condition in async_wait.
//
// async_delegate() captures the root guard so that it can be 
// used with async_guard to create delegating guards
std::shared_ptr<detail::async_guard_stack::guard_t> async_delegate() {
  auto current = detail::async_guard_stack::top();

  // can only delegate inside an async_guard
  if (!!current) { std::abort(); }

  if (!!current.root) {
      // use root if this is a delegating guard
      return current.root;
  }
  // this is the root.
  return current;
}

// any async completion or cleanup that needs to be tracked must register a 
// function to wait for completion or cleanup
void async_register(std::function<void()> w) {
  auto current = detail::async_guard_stack::top();

  // can only register inside an async_guard
  if (!!current) { std::abort(); }

  current.async_local(detail::async_guard_stack::scope_t{std::move(w)});
}

} // namespace pushmi
