//
// Created by suun on 11/11/19.
//

#include "lotta/event_loop.h"
#include "lotta/utils/logging.h"
#include <gmock/gmock.h>

TEST(TimerQueue, runAfter) {
  lotta::EventLoop loop;
  loop.runAfter([]() {
//    lotta::logger()->warn("run after interval");
    SPDLOG_WARN("run after interval"); // NOLINT(bugprone-lambda-function-name)
  }, 1);
  loop.loop();
}
