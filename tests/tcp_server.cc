//
// Created by suun on 11/22/19.
//

#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/tcp_server.h"
#include <gtest/gtest.h>

TEST(TcpServer, default_) {
  lotta::EventLoop loop;
  lotta::NetAddr addr(8001);
  lotta::TcpServer server(&loop, addr, "server");
  server.setThreadNum(8);
  server.start();
//  loop.runAfter([&l = loop] {
//    l.quit();
//  }, 1);
  loop.loop();
}
