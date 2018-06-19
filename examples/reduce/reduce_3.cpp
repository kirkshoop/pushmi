#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

#include <reduce.h>

using namespace pushmi::aliases;

auto inline_bulk_target() {
  return []<class IF, class RS, class Input, class F, class ShapeBegin, class ShapeEnd, class Out>(
      IF init,
      RS selector,
      Input input,
      F&& func,
      ShapeBegin sb,
      ShapeEnd se,
      Out out) {
        try {
          auto acc = init(input);
          for (decltype(sb) idx{sb}; idx != se; ++idx){
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
  std::fill(vec.begin(), vec.end(), 4);

  auto fortyTwo = mi::reduce(inline_bulk_target(), vec.begin(), vec.end(), 2, std::plus<>{});

  assert((2 + std::accumulate(vec.begin(), vec.end())) == fortyTwo);

  std::cout << "OK" << std::endl;
}