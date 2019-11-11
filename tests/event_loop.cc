//
// Created by suun on 11/7/19.
//
#include "lotta/event_loop.h"
#include "lotta/channel.h"
#include "lotta/utils/logging.h"
#include <gmock/gmock.h>
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

  itimerspec duration;
  bzero(&duration, sizeof(duration));
  duration.it_value.tv_sec = 5;
  timerfd_settime(timerFd, 0, &duration, nullptr);
  loop.loop();
  close(timerFd);
}
