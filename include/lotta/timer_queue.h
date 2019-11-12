//
// Created by suun on 11/10/19.
//

#ifndef LOTTA_TIMER_QUEUE_H
#define LOTTA_TIMER_QUEUE_H

#include "lotta/utils/noncopyable.h"
#include "lotta/event_loop.h"
#include "lotta/timer.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <map>

namespace lotta {

class Channel;

class TimerQueue : utils::noncopyable {
 public:
  using clock = Timer::clock;
  using duration = Timer::duration;
  using time_point = Timer::time_point;
  using Callback = Timer::Callback;
  using TimerEntry = std::pair<time_point, std::shared_ptr<Timer>>;
  using TimerMultiMap = std::multimap<time_point, std::shared_ptr<Timer>>;

  explicit TimerQueue(EventLoop *loop);
  ~TimerQueue();

  std::weak_ptr<Timer> addTimer(Callback, time_point when, double interval);
  void cancel(const std::weak_ptr<Timer> &timer);

 private:

  void handleTimer();
  std::vector<TimerEntry> expireTimers(time_point now);
  void resetTimers(time_point now, const std::vector<TimerEntry> &expired);

  void addTask(const std::shared_ptr<Timer> &timer);
  void cancelTask(const std::shared_ptr<Timer> &timer);
  bool pushTimerToMap(const std::shared_ptr<Timer> &timer);

  // timer event
  int fd_;
  std::unique_ptr<Channel> channel_;

  EventLoop *loop_;
  TimerMultiMap timers_;

  std::atomic_bool doingTimerCallback_;
  TimerMultiMap cancelledTimers_;
};

}

#endif //LOTTA_TIMER_QUEUE_H
