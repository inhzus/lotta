//
// Created by suun on 11/10/19.
//

#include "lotta/timer.h"

namespace lotta {

std::atomic_int64_t Timer::s_numTimer_ = 0;
void Timer::restart(Timer::time_point now) {
  if (repeat_) {
    std::chrono::duration<double> dur(interval_);
    when_ +=
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(dur);
  }
}

}
