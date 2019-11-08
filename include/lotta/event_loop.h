//
// Created by suun on 11/7/19.
//

#ifndef LIBNET_EVENT_LOOP_H
#define LIBNET_EVENT_LOOP_H
#include "lotta/utils/noncopyable.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <thread>

namespace lotta {
class EventLoop : noncopyable {
 public:
  EventLoop();
  ~EventLoop();
 private:
  bool looping_;
  const std::thread::id threadId_;
  std::shared_ptr<spdlog::logger> logger_;
};
}

#endif //LIBNET_EVENT_LOOP_H
