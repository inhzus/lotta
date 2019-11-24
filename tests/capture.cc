//
// Created by suun on 11/12/19.
//

#include "capture.h"
#include "lotta/event_loop.h"

namespace ti = testing::internal;

void split(const std::string &s,
           const std::string &delim,
           const std::function<void(const std::string &)> &f) {
  std::string::size_type m{}, n{};
  while (m != s.size()) {
    n = s.find(delim, m);
    f(s.substr(m, n - m));
    if (n == std::string::npos) { break; }
    m = n + delim.size();
  }
}

Record::Record(const std::string &level,
               std::string time,
               const std::string &threadId,
               std::string filename,
               const std::string &line,
               std::string msg) :
    rawTime_(std::move(time)),
    filename_(std::move(filename)),
    msg_(std::move(msg)) {
  switch (level[0]) {
    case 'T': level_ = spdlog::level::trace;
      break;
    case 'D': level_ = spdlog::level::debug;
      break;
    case 'I': level_ = spdlog::level::info;
      break;
    case 'W': level_ = spdlog::level::warn;
      break;
    case 'E': level_ = spdlog::level::err;
      break;
    case 'C': level_ = spdlog::level::critical;
      break;
    default: level_ = spdlog::level::off;
      break;
  }
  {
    int year, month, day, hour, min, sec, milli;
    sscanf(rawTime_.c_str(), "%d/%d/%d %d:%d:%d.%d", // NOLINT(cert-err34-c)
           &year, &month, &day, &hour, &min, &sec, &milli);
    std::tm tm{
        sec, min, hour, day, month - 1, year - 1900
    };
    std::time_t tt(std::mktime(&tm));
    time_ = clock::from_time_t(tt) + std::chrono::milliseconds(milli);
  }
  threadId_ = std::stoi(threadId);
  line_ = std::stoi(line);
}

std::vector<Record> Record::capture(const std::function<void()> &f) {
  ti::CaptureStdout();
  f();
  std::string logs = ti::GetCapturedStdout();
//  SPDLOG_INFO("captured begin\n{}end", logs);
  std::regex pattern(R"(^([^ ]+) \[([^\]]+)\])"
                     R"( (\d+) \[([^:]+):(\d+)\] (.*))");
  std::vector<Record> records;
  split(logs, "\n", [&p = pattern, &r = records](const std::string &s) {
    std::smatch match;
    if (!std::regex_search(s, match, p)) {
      return;
    }
    printf("%s\n", s.c_str());
    std::string level(match[1]),
        time(match[2]),
        threadId(match[3]),
        filename(match[4]),
        line(match[5]),
        msg(match[6]);
    r.emplace_back(level, time, threadId, filename, line, msg);
  });
  return records;
}
::testing::AssertionResult Record::diff(
    const Record &lhs, const Record &rhs, int milliseconds) {
  int dur = std::chrono::duration_cast<
      std::chrono::milliseconds>(rhs.time_ - lhs.time_).count();
  if (dur - milliseconds < 10 && milliseconds - dur < 10) {
    return ::testing::AssertionSuccess();
  }
  return ::testing::AssertionFailure()
      << lhs.rawTime_ << " diff " << rhs.rawTime_
      << " not match " << milliseconds << " ms";

}

TEST(Capture, Record) {
  auto records = Record::capture([]() {
    lotta::EventLoop loop;
  });
  std::vector<Record> chRecords;
  std::regex channelPattern(R"(fd: \d+, events: .*)");
  std::copy_if(
      records.begin(), records.end(),
      std::back_inserter(chRecords),
      [&p = channelPattern](const Record &record) {
        return std::regex_match(record.msg_, p);
      });
  EXPECT_EQ(4, chRecords.size());
  EXPECT_TRUE(Record::diff(chRecords[0], chRecords[1], 0));
}
