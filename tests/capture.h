//
// Created by suun on 11/12/19.
//

#ifndef LOTTA_TESTS_HEADER_H
#define LOTTA_TESTS_HEADER_H

#include "lotta/utils/logging.h"
#include <gmock/gmock.h>
#include <chrono>
#include <regex>

class Record {
 public:
  using clock = std::chrono::system_clock;
  using time_point = clock::time_point;
  spdlog::level::level_enum level_;
  std::string rawTime_;
  time_point time_;
  size_t threadId_;
  std::string filename_;
  size_t line_;
  std::string msg_;

  Record(const std::string &level,
         std::string time,
         const std::string &threadId,
         std::string filename,
         const std::string &line,
         std::string msg);

  static std::vector<Record> capture(const std::function<void()> &);
  static ::testing::AssertionResult diff(
      const Record &lhs, const Record &rhs, int milliseconds);
};

#endif //LOTTA_TESTS_HEADER_H
