#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

#include <pool.h>

#include <request_via.h>

#include <pushmi/o/transform.h>

using namespace pushmi::aliases;

template<class Io>
auto io_operation(Io io) {
    return io | 
      op::transform([](auto){ return 42; }) |
      op::request_via();
}

int main()
{
  mi::pool cpuPool{std::max(1u,std::thread::hardware_concurrency())};
  mi::pool ioPool{std::max(1u,std::thread::hardware_concurrency())};

  io_operation(ioPool.executor()).via([cpu = cpuPool.executor()]{ return cpu; }) | 
    op::submit();

  std::cout << "OK" << std::endl;
}



