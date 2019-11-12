//
// Created by suun on 11/11/19.
//

#include "lotta/event_loop.h"
#include "lotta/utils/logging.h"
#include <gmock/gmock.h>

TEST(TimerQueue, runAfter) {
  lotta::EventLoop loop;
  loop.runAfter([]() {
    SPDLOG_WARN("after 1 second");
  }, 1);
  loop.runAfter([&l = loop]() {
    l.quit();
    SPDLOG_WARN("loop quit");
  }, 2);
  loop.loop();
}

TEST(TimerQueue, runEvery) {
  lotta::EventLoop loop;
  auto timer = loop.runEvery([]() {
    SPDLOG_WARN("every 1 second");
  }, 1);
  loop.runAfter([&t = timer, &l = loop]() {
    l.cancel(t);
    SPDLOG_WARN("task cancelled after 3.1 seconds");
  }, 3.1);
  loop.runAfter([&l = loop]() {
    l.quit();
    SPDLOG_WARN("loop quit after 5 seconds");
  }, 5);
  loop.loop();
}
