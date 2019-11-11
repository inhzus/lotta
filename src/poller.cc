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
      SPDLOG_CRITICAL("logger initialized"); // NOLINT
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
