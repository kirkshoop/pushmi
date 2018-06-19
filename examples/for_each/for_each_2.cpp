#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

#include <pool.h>
#include <bulk.h>
#include <for_each.h>

#include <pushmi/single_deferred.h>
#include <pushmi/o/submit.h>
#include <pushmi/o/just.h>

using namespace pushmi::aliases;

template<class Executor, class Allocator = std::allocator<char>>
auto naive_executor_bulk_target(Executor e, Allocator a = Allocator{}) {
  return [e, a]<class IF, class RS, class Input, class F, class Shape, class Out>(
      IF init,
      RS selector,
      Input input,
      F&& func,
      Shape s,
      Out out) {
        try {
          typename std::allocator_traits<Allocator>::template rebind_alloc<char> allocState(a);
          auto shared_state = std::allocate_shared<
            std::tuple<
              std::exception_ptr, // first exception
              Out, // destination
              RS, // selector
              F, // func
              decltype(init(input)), // accumulation
              std::atomic<decltype(s)>, // pending
              std::atomic<decltype(s)> // exception count (protects assignment to first exception)
            >>(allocState, std::exception_ptr{}, std::move(out), std::move(selector), std::move(func), init(input), s, 0);
          for (decltype(s) idx{}; idx < s; ++idx){
              e | op::submit([shared_state, idx](){
                try {
                  // func(accumulation, idx)
                  std::get<3>(*shared_state)(std::get<4>(*shared_state), idx);
                } catch(...) {
                  // exception count
                  if (std::get<6>(*shared_state)++ == 0) {
                    // store first exception
                    std::get<0>(*shared_state) = std::current_exception();
                  }
                }
                // pending
                if (--std::get<5>(*shared_state) == decltype(s){}) {
                  // first exception
                  if (std::get<0>(*shared_state)) {
                    mi::set_error(std::get<1>(*shared_state), std::get<0>(*shared_state));
                    return;
                  }
                  try {
                    // selector(accumulation)
                    auto result = std::get<2>(*shared_state)(std::move(std::get<4>(*shared_state)));
                    mi::set_value(std::get<1>(*shared_state), std::move(result));
                  } catch(...) {
                    mi::set_error(std::get<1>(*shared_state), std::current_exception());
                  }
                }
              });
          }
        } catch(...) {
          mi::set_error(out, std::current_exception());
        }
    };
}

int main()
{
  mi::pool p{std::max(1u,std::thread::hardware_concurrency())};

  std::vector<int> vec(10);

  mi::for_each(naive_executor_bulk_target(p.executor()), vec.begin(), vec.end(), [](int& x){
    x = 42;
  });

  assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));

  std::cout << "OK" << std::endl;

  p.stop();
  p.wait();
}
