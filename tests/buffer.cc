//
// Created by suun on 11/19/19.
//

#include "lotta/buffer.h"
#include "lotta/utils/logging.h"
#include <gmock/gmock.h>

TEST(Buffer, default_) {
  lotta::Buffer buffer;
  EXPECT_EQ(0, buffer.readable());
  EXPECT_EQ(lotta::Buffer::kInitLen, buffer.writable());

  const size_t kInit = 200;
  const std::string sx(kInit, 'x');
  buffer.append(lotta::Slice(sx));
  EXPECT_EQ(sx.size(), buffer.readable());

  const size_t kRetrieved = 50;
  const std::string ssx(buffer.retrieve(kRetrieved).toString());
  EXPECT_EQ(kRetrieved, ssx.size());
  EXPECT_EQ(sx.size() - ssx.size(), buffer.readable());
  EXPECT_EQ(ssx, std::string(kRetrieved, 'x'));

  buffer.append(lotta::Slice(sx));
  EXPECT_EQ(sx.size() * 2 - ssx.size(), buffer.readable());

  const std::string r(buffer.retrieve().toString());
  EXPECT_EQ(kInit * 2 - kRetrieved, r.size());
  EXPECT_EQ(0, buffer.readable());
}

TEST(Buffer, makeSpaceOuter) {
  lotta::Buffer buffer;

  buffer.append(lotta::Slice(std::string(400, 'y')));
  EXPECT_EQ(400, buffer.readable());
  buffer.retrieve(50);
  EXPECT_EQ(350, buffer.readable());
  buffer.append(lotta::Slice(std::string(1000, 'z')));
  EXPECT_EQ(1350, buffer.readable());
  SPDLOG_INFO("buffer writable: {}", buffer.writable());
  buffer.retrieve();
  EXPECT_EQ(0, buffer.readable());
}

TEST(Buffer, makeSpaceInner) {
  lotta::Buffer buffer;
  buffer.append(lotta::Slice(std::string(800, 'y')));
  EXPECT_EQ(800, buffer.readable());
  buffer.retrieve(500);
  EXPECT_EQ(300, buffer.readable());
  buffer.append(lotta::Slice(std::string(300, 'z')));
  EXPECT_EQ(600, buffer.readable());
}

TEST(Buffer, peek) {
  lotta::Buffer buffer;
  const char *lotta = "lotta";
  buffer.append(lotta);
  EXPECT_EQ(lotta, buffer.toString());

  lotta::Buffer b;
  b.swap(buffer);
  EXPECT_TRUE(buffer.toSlice().empty());
  EXPECT_EQ(lotta, b.toString());
  lotta::Buffer ctor(b);
  EXPECT_EQ(lotta, ctor.toString());
  EXPECT_EQ(lotta, b.toString());
}

TEST(Buffer, retrieve) {
  lotta::Buffer buffer;
  buffer.append("01234\n56789\r\n");
  EXPECT_EQ("01234", buffer.retrieveUntil("\n").toString());
  EXPECT_EQ("56789", buffer.retrieveUntil("\r\n").toString());
}

TEST(Buffer, revoke) {
  lotta::Buffer buffer;
  buffer.append("01234");
  buffer.discard(2);
  EXPECT_EQ("012", buffer.toString());
  buffer.append("3456789");
  EXPECT_EQ("0123", buffer.retrieveUntil("45").toString());
  buffer.revoke(6);
  EXPECT_EQ("0123456789", buffer.toString());
}
