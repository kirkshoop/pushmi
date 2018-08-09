
#include <vector>

#include "pushmi/o/just.h"
#include "pushmi/o/on.h"
#include "pushmi/o/transform.h"
#include "pushmi/o/tap.h"
#include "pushmi/o/via.h"
#include "pushmi/o/submit.h"
#include "pushmi/o/from.h"
#include "pushmi/o/for_each.h"

#include "pushmi/trampoline.h"
#include "pushmi/new_thread.h"

#include "pushmi/none.h"
#include "pushmi/entangle.h"

#include "pool.h"

using namespace pushmi::aliases;

struct inline_executor_none;

template<class R>
struct countdown {
  countdown(int& c)
      : counter(&c) {}

  int* counter;

  template <class ExecutorRef>
  void operator()(ExecutorRef exec) const;
};

template<class R>
template <class ExecutorRef>
void countdown<R>::operator()(ExecutorRef exec) const {
  if (--*counter > 0) {
    exec | op::submit(R{}(*this));
  }
}

using countdownsingle = countdown<mi::make_single_fn>;
using countdownflowsingle = countdown<mi::make_flow_single_fn>;
using countdownmany = countdown<mi::make_many_fn>;
using countdownflowmany = countdown<mi::make_flow_many_fn>;

struct countdownnone {
  countdownnone(int& c)
      : counter(&c) {}

  int* counter;

  void operator()() const;
};

struct inline_executor {
    using properties = mi::property_set<mi::is_sender<>, mi::is_single<>>;
    template<class Out>
    void submit(Out out) {
      ::mi::set_value(out, *this);
    }
};

struct inline_time_executor {
    using properties = mi::property_set<mi::is_time<>, mi::is_single<>>;

    std::chrono::system_clock::time_point now() { return std::chrono::system_clock::now(); }
    template<class Out>
    void submit(std::chrono::system_clock::time_point at, Out out) {
      std::this_thread::sleep_until(at);
      ::mi::set_value(out, *this);
    }
};

struct inline_executor_none {
    using properties = mi::property_set<mi::is_sender<>, mi::is_none<>>;
    template<class Out>
    void submit(Out out) {
      ::mi::set_done(out);
    }
};

void countdownnone::operator()() const {
  if (--*counter > 0) {
    inline_executor_none{} | op::submit(mi::make_none(*this));
  }
}

template<class CancellationFactory>
struct inline_executor_flow_single {
    CancellationFactory cf;

    using properties = mi::property_set<mi::is_sender<>, mi::is_flow<>, mi::is_single<>>;
    template<class Out>
    void submit(Out out) {

      auto tokens = cf();

      using Stopper = decltype(tokens.second);
      struct Data : mi::none<> {
        explicit Data(Stopper stopper) : stopper(std::move(stopper)) {}
        Stopper stopper;
      };
      auto up = mi::MAKE(none)(
          Data{std::move(tokens.second)},
          [](auto& data, auto e) noexcept {
            auto both = lock_both(data.stopper);
            (*(both.first))(both.second);
          },
          [](auto& data) {
            auto both = lock_both(data.stopper);
            (*(both.first))(both.second);
          });

    // pass reference for cancellation.
    ::mi::set_starting(out, std::move(up));

    auto both = lock_both(tokens.first);
    if (!!both.first && !*(both.first)) {
      ::mi::set_value(out, *this);
    } else {
      // cancellation is not an error
      ::mi::set_done(out);
    }
  }
};

struct shared_cancellation_factory{
  auto operator()(){
    // boolean cancellation
    bool stop = false;
    auto set_stop = [](auto& stop) {
      if (!!stop) {
        *stop = true;
      }
    };
    return mi::shared_entangle(stop, set_stop);
  }
};
using inline_executor_flow_single_shared = inline_executor_flow_single<shared_cancellation_factory>;

struct entangled_cancellation_factory{
  auto operator()(){
    // boolean cancellation
    bool stop = false;
    auto set_stop = [](auto& stop) {
      if (!!stop) {
        *stop = true;
      }
    };
    return mi::entangle(stop, set_stop);
  }
};
using inline_executor_flow_single_entangled = inline_executor_flow_single<entangled_cancellation_factory>;

struct inline_executor_flow_single_ignore {
    using properties = mi::property_set<mi::is_sender<>, mi::is_flow<>, mi::is_single<>>;
    template<class Out>
    void submit(Out out) {
      // pass reference for cancellation.
      ::mi::set_starting(out, mi::none<>{});

      ::mi::set_value(out, *this);
    }
};

struct inline_executor_flow_many {
  inline_executor_flow_many()
    : counter(nullptr) {}
  inline_executor_flow_many(int& c)
      : counter(&c) {}

  int* counter;

  using properties = mi::property_set<mi::is_sender<>, mi::is_flow<>, mi::is_many<>>;

  template<class Out>
  void submit(Out out) {

    // boolean cancellation
    struct producer {
      producer(Out out, bool s) : out(std::move(out)), stop(s) {}
      Out out;
      std::atomic<bool> stop;
    };
    auto p = std::make_shared<producer>(std::move(out), false);

    struct Data : mi::many<> {
      explicit Data(std::shared_ptr<producer> p) : p(std::move(p)) {}
      std::shared_ptr<producer> p;
    };

    auto up = mi::MAKE(many)(
        Data{p},
        [counter = this->counter](auto& data, auto requested) {
          if (requested < 1) {return;}
          // check boolean to select signal
          while (!data.p->stop && !!counter && --*counter > 0) {
            ::mi::set_next(data.p->out, inline_executor_flow_many{*counter});
          }
          ::mi::set_done(data.p->out);
        },
        [](auto& data, auto e) noexcept {
          data.p->stop.store(true);
          ::mi::set_done(data.p->out);
        },
        [](auto& data) {
          data.p->stop.store(true);
          ::mi::set_done(data.p->out);
        });

    // pass reference for cancellation.
    ::mi::set_starting(p->out, std::move(up));
  }
};

struct inline_executor_flow_many_ignore {
    using properties = mi::property_set<mi::is_sender<>, mi::is_flow<>, mi::is_many<>>;
    template<class Out>
    void submit(Out out) {
      // pass reference for cancellation.
      ::mi::set_starting(out, mi::many<>{});

      ::mi::set_next(out, *this);

      ::mi::set_done(out);
    }
};

struct inline_executor_many {
    using properties = mi::property_set<mi::is_sender<>, mi::is_many<>>;
    template<class Out>
    void submit(Out out) {
      ::mi::set_next(out, *this);
      ::mi::set_done(out);
    }
};


#define concept Concept
#include <nonius/nonius.h++>

NONIUS_BENCHMARK("inline 1'000 none", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_none{};
  using IE = decltype(ie);
  countdownnone none{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_none(none));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 single", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor{};
  using IE = decltype(ie);
  countdownsingle single{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_single(single));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 time single", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_time_executor{};
  using IE = decltype(ie);
  countdownsingle single{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_single(single));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 many", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_many{};
  using IE = decltype(ie);
  countdownmany many{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_many(many));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 flow_single shared", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_flow_single_shared{};
  using IE = decltype(ie);
  countdownflowsingle flowsingle{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_flow_single(flowsingle));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 flow_single entangle", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_flow_single_entangled{};
  using IE = decltype(ie);
  countdownflowsingle flowsingle{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_flow_single(flowsingle));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 flow_single ignore cancellation", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_flow_single_ignore{};
  using IE = decltype(ie);
  countdownflowsingle flowsingle{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_flow_single(flowsingle));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 flow_many", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_flow_many{};
  using IE = decltype(ie);
  countdownflowmany flowmany{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_flow_many(flowmany));
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1 flow_many with 1'000 values", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_flow_many{counter};
  using IE = decltype(ie);
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_flow_many());
    return counter;
  });
})

NONIUS_BENCHMARK("inline 1'000 flow_many ignore cancellation", [](nonius::chronometer meter){
  int counter = 0;
  auto ie = inline_executor_flow_many_ignore{};
  using IE = decltype(ie);
  countdownflowmany flowmany{counter};
  meter.measure([&]{
    counter = 1'000;
    ie | op::submit(mi::make_flow_many(flowmany));
    return counter;
  });
})

NONIUS_BENCHMARK("trampoline static derecursion 1'000'000", [](nonius::chronometer meter){
  int counter = 0;
  auto tr = mi::trampoline();
  using TR = decltype(tr);
  countdownsingle single{counter};
  meter.measure([&]{
    counter = 1'000'000;
    tr | op::submit(single);
    return counter;
  });
})

NONIUS_BENCHMARK("trampoline virtual derecursion 1'000'000", [](nonius::chronometer meter){
  int counter = 0;
  auto tr = mi::trampoline();
  using TR = decltype(tr);
  std::function<void(mi::any_time_executor_ref<> exec)> recurse{countdownsingle{counter}};
  meter.measure([&]{
    counter = 1'000'000;
    tr | op::submit([&](auto exec) { recurse(exec); });
    return counter;
  });
})

NONIUS_BENCHMARK("trampoline flow_many_sender 1'000'000", [](nonius::chronometer meter){
  int counter = 0;
  auto tr = mi::trampoline();
  using TR = decltype(tr);
  std::vector<int> values(1'000'000);
  std::iota(values.begin(), values.end(), 1);
  auto f = op::flow_from(values, tr) | op::tap([&](int){--counter;});
  meter.measure([&]{
    counter = 1'000'000;
    f | op::for_each(mi::make_many());
    return counter;
  });
})

NONIUS_BENCHMARK("pool{1} blocking_submit 1'000'000", [](nonius::chronometer meter){
  mi::pool pl{std::max(1u,std::thread::hardware_concurrency())};
  auto pe = pl.executor();
  using PE = decltype(pe);
  int counter = 0;
  countdownsingle single{counter};
  meter.measure([&]{
    counter = 1'000'000;
    pe | op::blocking_submit(single);
    return counter;
  });
})

NONIUS_BENCHMARK("new thread blocking_submit 1'000'000", [](nonius::chronometer meter){
  auto nt = mi::new_thread();
  using NT = decltype(nt);
  int counter = 0;
  countdownsingle single{counter};
  meter.measure([&]{
    counter = 1'000'000;
    nt | op::blocking_submit(single);
    return counter;
  });
})
