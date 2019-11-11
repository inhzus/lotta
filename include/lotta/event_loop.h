//
// Created by suun on 11/7/19.
//

#ifndef LOTTA_EVENT_LOOP_H
#define LOTTA_EVENT_LOOP_H
#include "lotta/utils/noncopyable.h"
#include <atomic>
#include <cstddef> // size_t
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace lotta {

class Channel;
class Poller;
class Timer;
class TimerQueue;

class EventLoop : utils::noncopyable {
 public:
  using Function = std::function<void()>;

  EventLoop();
  ~EventLoop();
  void loop();
  void quit();

  void updateChannel(Channel *);

  void exec(Function);
  void pushQueue(Function);
  void doFuncQueue();

  std::weak_ptr<Timer> runAfter(Function, double interval);


  void assertTheSameThread() const;
 private:
  [[nodiscard]] bool isTheSameThread() const;

  void wakeup();
  void handleWakeup();

  bool looping_;
  std::atomic_bool quit_;
  const size_t threadId_;
  std::vector<Channel *> activeChannels_;
  std::unique_ptr<Poller> poller_;

  // wakeup event
  int wakeupFd_;
  std::unique_ptr<Channel> wakeChannel_;

  // pending queue
  std::mutex funcQueueMtx_;
  std::atomic_bool doingFuncQueue_;
  std::vector<Function> funcQueue_;

  // timer queue
  std::unique_ptr<TimerQueue> timerQueue_;
};

}

#endif //LOTTA_EVENT_LOOP_H
