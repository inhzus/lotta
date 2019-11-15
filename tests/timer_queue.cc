//
// Created by suun on 11/11/19.
//

#include "capture.h"
#include "lotta/event_loop.h"
#include "lotta/utils/logging.h"
#include <gtest/gtest.h>

TEST(TimerQueue, runAfter) {
  lotta::EventLoop loop;
  loop.runAfter([]() {
    SPDLOG_WARN("after 1 second");
  }, 1);
  loop.runAfter([&l = loop]() {
    l.quit();
    SPDLOG_WARN("loop quit");
  }, 2);
  auto records = Record::capture([&loop = loop]() {
    loop.loop();
  });
  std::vector<Record> samples;
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(samples),
      [](const Record &record) {
        return record.msg_.rfind("looping") != std::string::npos ||
            record.msg_ == "after 1 second" ||
            record.msg_ == "loop quit";
      });
  ASSERT_EQ(3, samples.size());
  EXPECT_TRUE(Record::diff(samples[0], samples[1], 1000));
  EXPECT_TRUE(Record::diff(samples[0], samples[2], 2000));
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
