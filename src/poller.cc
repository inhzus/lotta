//
// Created by suun on 11/9/19.
//

#include "lotta/poller.h"
#include "lotta/poll_poller.h"
#include "spdlog/spdlog.h"

namespace lotta {

namespace utils::logging {

class Initializer {
 public:
  Initializer() {
    static int i = []() {
      ::spdlog::set_level(::spdlog::level::trace);
      // format as: "T(colored short for level) [12D(day of month)
      // 15:04:05:678] 9691(thread id) [poller.cc:20] actual logging message"
      ::spdlog::set_pattern("%^%L%$ [%dD %T:%e] %t [%s:%#] %v");
      SPDLOG_INFO("logger initialized"); // NOLINT
      // (bugprone-lambda-function-name)
      return 0;
    }();
    (void) i;
  }
};

Initializer initializer;

}

Poller::Poller(EventLoop *loop)
    : loop_(loop) {}
Poller::~Poller() = default;

std::unique_ptr<Poller> Poller::get(EventLoop *loop) {
  return std::unique_ptr<Poller>(new PollPoller(loop));
}

}
