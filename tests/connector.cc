//
// Created by suun on 11/26/19.
//

#include "lotta/connector.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include <gtest/gtest.h>

using namespace lotta;

TEST(Connector, default_) {
  EventLoop loop;
  NetAddr addr("127.0.0.1", 8001);
  Connector connector(&loop, addr);
  connector.start();
  loop.runAfter([&l = loop] {
    l.quit();
  }, 1);
  loop.loop();
}
