//
// Created by suun on 11/10/19.
//

#include "lotta/timer.h"

namespace lotta {

void Timer::restart() {
  if (repeat_) {
    std::chrono::duration<double> dur(interval_);
    when_ +=
        std::chrono::duration_cast<duration>(dur);
  }
}

}
