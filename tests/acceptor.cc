//
// Created by suun on 11/14/19.
//

#include "capture.h"
#include "lotta/acceptor.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include <gtest/gtest.h>

TEST(Acceptor, default_) {
  lotta::EventLoop loop;
  lotta::NetAddr addr(8001, true);
  lotta::Acceptor acceptor(&loop, addr);
  acceptor.setConnCallback(
      [](int fd, const lotta::NetAddr &addr) {
        SPDLOG_WARN("fd {} captured from {}", fd, addr.addr());
      });
  acceptor.listen();
  loop.runAfter([]() {
    if (fork() == 0) {
      SPDLOG_WARN("connect acceptor via nc");
      execl("/usr/bin/nc", "-z", "localhost", "8001", nullptr);
      exit(0);
    }
  }, 1);
  loop.runAfter([&l = loop]() {
    l.quit();
  }, 1.5);
  std::vector<Record> records = Record::capture([&l = loop]() {
    l.loop();
  }), bind, accepted;
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(bind),
      [](const Record &record) {
        return record.msg_ == "connect acceptor via nc";
      });
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(accepted),
      [](const Record &record) {
        return record.msg_.find("captured from") != std::string::npos;
      });
  EXPECT_EQ(1, bind.size());
  EXPECT_EQ(1, accepted.size());
  if (!bind.empty() && !accepted.empty()) {
    ASSERT_TRUE(Record::diff(bind[0], accepted[0], 5));
  }
}
