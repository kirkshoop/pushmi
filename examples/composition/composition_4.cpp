#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

#include <pool.h>

#include <request_via.h>

#include <pushmi/o/tap.h>
#include <pushmi/o/transform.h>

using namespace pushmi::aliases;

template<class Io>
auto io_operation(Io io) {
    return io | 
      op::transform([](auto){ return 42; }) |
      op::tap([](int v){ printf("io pool producing, %d\n", v); }) |
      op::request_via();
}

int main()
{
  mi::pool cpuPool{std::max(1u,std::thread::hardware_concurrency())};
  mi::pool ioPool{std::max(1u,std::thread::hardware_concurrency())};

  auto io = ioPool.executor();
  auto cpu = cpuPool.executor();

  io_operation(io).via([cpu]{ return cpu; }) | 
    op::tap([](int v){ printf("cpu pool processing, %d\n", v); }) |
    op::submit();

  ioPool.wait();
  cpuPool.wait();

  std::cout << "OK" << std::endl;
}



