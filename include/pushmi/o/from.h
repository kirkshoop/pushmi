#pragma once
// Copyright (c) 2018-present, Facebook, Inc.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "../many_sender.h"
#include "../flow_many_sender.h"
#include "../trampoline.h"
#include "extension_operators.h"
#include "submit.h"

namespace pushmi {

PUSHMI_CONCEPT_DEF(
  template (class R)
  concept Range,
    requires (R&& r) (
      implicitly_convertible_to<bool>(std::begin(r) == std::end(r))
    )
);

namespace operators {

PUSHMI_INLINE_VAR constexpr struct from_fn {
private:
  template <class I, class S>
  struct out_impl {
    I begin_;
    S end_;
    PUSHMI_TEMPLATE(class Out)
      (requires ManyReceiver<Out, typename std::iterator_traits<I>::value_type>)
    void operator()(Out out) const {
      auto c = begin_;
      for (; c != end_; ++c) {
        ::pushmi::set_next(out, *c);
      }
      ::pushmi::set_done(out);
    }
  };
public:
  PUSHMI_TEMPLATE(class I, class S)
    (requires
      DerivedFrom<
          typename std::iterator_traits<I>::iterator_category,
          std::forward_iterator_tag>)
  auto operator()(I begin, S end) const {
    return make_many_sender(out_impl<I, S>{begin, end});
  }

  PUSHMI_TEMPLATE(class R)
    (requires Range<R>)
  auto operator()(R&& range) const {
    return (*this)(std::begin(range), std::end(range));
  }
} from {};

PUSHMI_INLINE_VAR constexpr struct flow_from_fn {
private:
  template <class I, class S, class Exec>
  struct out_impl {
    I begin_;
    S end_;
    mutable Exec exec_;
    PUSHMI_TEMPLATE(class Out)
      (requires ManyReceiver<Out, typename std::iterator_traits<I>::value_type>)
    void operator()(Out out) const {

      // boolean cancellation
      struct producer {
        producer(I begin, S end, Out out, Exec exec, bool s) : c(begin), end(end), out(std::move(out)), exec(std::move(exec)), stop(s) {}
        I c;
        S end;
        Out out;
        Exec exec;
        std::atomic<bool> stop;
      };
      auto p = std::make_shared<producer>(begin_, end_, std::move(out), exec_, false);

      struct Data : many<> {
        explicit Data(std::shared_ptr<producer> p) : p(std::move(p)) {}
        std::shared_ptr<producer> p;
      };

      ::pushmi::submit(exec_,
        ::pushmi::now(exec_),
        make_single([p](auto exec) mutable {
          auto up = make_many(
            Data{p},
            [](auto& data, auto requested) {
              if (requested < 1) {return;}
              // submit work to exec
              ::pushmi::submit(data.p->exec,
                ::pushmi::now(data.p->exec),
                make_single([p = data.p, requested](auto)  {
                  auto remaining = requested;
                  // this loop is structured to work when there is re-entrancy
                  // out.next in the loop may call up.next. to handle this the
                  // state of p->c must be captured and the remaining and p->c
                  // must be changed before out.next is called.
                  while (remaining-- > 0 && !p->stop && p->c != p->end) {
                    auto i = (p->c)++;
                    ::pushmi::set_next(p->out, *i);
                  }
                  if (p->c == p->end) {
                    ::pushmi::set_done(p->out);
                  }
                }));
            },
            [](auto& data, auto e) noexcept {
              data.p->stop.store(true);
              ::pushmi::submit(data.p->exec,
                ::pushmi::now(data.p->exec),
                make_single([p = data.p](auto)  {
                  ::pushmi::set_done(p->out);
                }));
            },
            [](auto& data) {
              data.p->stop.store(true);
              ::pushmi::submit(data.p->exec,
                ::pushmi::now(data.p->exec),
                make_single([p = data.p](auto)  {
                  ::pushmi::set_done(p->out);
                }));
            });

          // pass reference for cancellation.
          ::pushmi::set_starting(p->out, std::move(up));
        }));
    }
  };
public:
  PUSHMI_TEMPLATE(class I, class S)
    (requires
      DerivedFrom<
          typename std::iterator_traits<I>::iterator_category,
          std::forward_iterator_tag>)
  auto operator()(I begin, S end) const {
    return (*this)(begin, end, trampoline());
  }

  PUSHMI_TEMPLATE(class R)
    (requires Range<R>)
  auto operator()(R&& range) const {
    return (*this)(std::begin(range), std::end(range), trampoline());
  }

  PUSHMI_TEMPLATE(class I, class S, class Exec)
    (requires
      DerivedFrom<
          typename std::iterator_traits<I>::iterator_category,
          std::forward_iterator_tag> &&
      Sender<Exec> && Time<Exec> && Single<Exec>)
  auto operator()(I begin, S end, Exec&& exec) const {
    return make_flow_many_sender(out_impl<I, S, Exec>{begin, end, (Exec&&) exec});
  }

  PUSHMI_TEMPLATE(class R, class Exec)
    (requires Range<R> && Sender<Exec> && Time<Exec> && Single<Exec>)
  auto operator()(R&& range, Exec&& exec) const {
    return (*this)(std::begin(range), std::end(range), (Exec&&) exec);
  }
} flow_from {};

} // namespace operators

} // namespace pushmi
