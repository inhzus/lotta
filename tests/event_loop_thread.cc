//
// Created by suun on 11/24/19.
//

#include "lotta/event_loop_thread.h"
#include "lotta/event_loop.h"
#include "lotta/utils/logging.h"
#include "capture.h"
#include <gtest/gtest.h>

using namespace lotta;

TEST(EventLoopThread, default_) {
  {
    EventLoopThread loop;
  }

  std::vector<Record> records = Record::capture([] {
    EventLoopThread thread;
    EventLoop *loop = thread.start();
    loop->exec([l = loop] {
      SPDLOG_WARN("quit");
      l->quit();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }), capture;
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(capture),
      [](const Record &record) {
        return record.msg_ == "quit";
      });
  EXPECT_EQ(1, capture.size());

  records = Record::capture([] {
    EventLoopThread thread;
    EventLoop *loop = thread.start();
    loop->exec([] {
      SPDLOG_WARN("running");
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });
  capture.clear();
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(capture),
      [](const Record &record) {
        return record.msg_ == "running";
      }
  );
  EXPECT_EQ(1, capture.size());

}
