//
// Created by suun on 11/7/19.
//

#ifndef LOTTA_EVENT_LOOP_H
#define LOTTA_EVENT_LOOP_H
#include "lotta/utils/noncopyable.h"
#include <atomic>
#include <cstddef> // size_t
#include <memory>
#include <vector>

namespace lotta {

class Channel;
class Poller;

class EventLoop : utils::noncopyable {
 public:
  EventLoop();
  ~EventLoop();
  void run();
  void quit();

  void updateChannel(Channel *);
  void assertThreadLoop() const;
 private:
  [[nodiscard]] bool isThreadLoop() const;

  bool looping_;
  std::atomic_bool quit_;
  const size_t threadId_;
  std::vector<Channel *> activeChannels_;
  std::unique_ptr<Poller> poller_;
};

}

#endif //LOTTA_EVENT_LOOP_H
