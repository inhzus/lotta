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
      repeat_(interval > 0), sequence_(++s_numTimer_) {}
  void run() const { callback_(); }
  void restart(time_point now);

  [[nodiscard]] time_point getWhen() const { return when_; }
  [[nodiscard]] bool isRepeat() const { return repeat_; }
  [[nodiscard]] int64_t getSequence() const { return sequence_; }

 private:
  const Callback callback_;
  time_point when_;
  const double interval_;
  const bool repeat_;
  const int64_t sequence_;

  static std::atomic_int64_t s_numTimer_;

};

}

#endif //LOTTA_TIMER_H
