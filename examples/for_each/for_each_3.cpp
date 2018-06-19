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

auto inline_bulk_target() {
  return []<class IF, class RS, class Input, class F, class Shape, class Out>(
      IF init,
      RS selector,
      Input input,
      F&& func,
      Shape s,
      Out out) {
        try {
          auto acc = init(input);
          for (decltype(s) idx{}; idx < s; ++idx){
              func(acc, idx);
          }
          auto result = selector(std::move(acc));
          mi::set_value(out, std::move(result));
        } catch(...) {
          mi::set_error(out, std::current_exception());
        }
      };
}

int main()
{
  std::vector<int> vec(10);

  mi::for_each(inline_bulk_target(), vec.begin(), vec.end(), [](int& x){
    x = 42;
  });

  assert(std::count(vec.begin(), vec.end(), 42) == static_cast<int>(vec.size()));

  std::cout << "OK" << std::endl;
}
