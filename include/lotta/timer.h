//
// Created by suun on 11/10/19.
//

#ifndef LOTTA_TIMER_H
#define LOTTA_TIMER_H

#include <atomic>
#include <chrono>
#include <functional>
#include <utility>

namespace lotta {

class Timer {
 public:
  using clock = std::chrono::steady_clock;
  using duration = clock::duration;
  using time_point = clock::time_point;
  using Callback = std::function<void()>;

  Timer(Callback callback, time_point when, double interval) :
      callback_(std::move(callback)), when_(when), interval_(interval),
      repeat_(interval > 0) {}
  void run() const { callback_(); }
  void restart();

  [[nodiscard]] time_point when() const { return when_; }
  [[nodiscard]] bool repeat() const { return repeat_; }

 private:
  const Callback callback_;
  time_point when_;
  const double interval_;
  const bool repeat_;

  static std::atomic_int64_t s_numTimer_;

};

}

#endif //LOTTA_TIMER_H
