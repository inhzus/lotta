//
// Created by suun on 11/7/19.
//

#include "capture.h"
#include "lotta/event_loop.h"
#include "lotta/channel.h"
#include "lotta/utils/logging.h"
#include <gtest/gtest.h>
#include <sys/timerfd.h>

TEST(EventLoop, constructor) {
  lotta::EventLoop loop;
//  loop.loop();
}

TEST(EventLoop, timerChannel) {
  lotta::EventLoop loop;
  int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  lotta::Channel channel(&loop, timerFd);
  channel.setReadCallback([l = &loop]() {
    SPDLOG_WARN("timeout"); // NOLINT(bugprone-lambda-function-name)
    l->quit();
  });
  channel.enableReading();

  itimerspec duration{{}, {}};
  duration.it_value.tv_sec = 1;
  timerfd_settime(timerFd, 0, &duration, nullptr);
  auto records = Record::capture([&l = loop]() {
    l.loop();
  });
  close(timerFd);
  std::vector<Record> samples;
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(samples),
      [](const Record &record) {
        return record.msg_.rfind("looping") != std::string::npos ||
            record.msg_ == "timeout";
      });
  ASSERT_EQ(2, samples.size());
  ASSERT_TRUE(Record::diff(samples[0], samples[1], 1000));
}
