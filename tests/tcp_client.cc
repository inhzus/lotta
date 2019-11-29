//
// Created by suun on 11/26/19.
//

#include "lotta/tcp_client.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/tcp_connection.h"
#include <gtest/gtest.h>

using namespace lotta;

TEST(TcpClient, default_) {
  EventLoop loop;
  NetAddr addr("127.0.0.1", 8001);
  TcpClient client(&loop, addr, "client");
  client.connect();
  loop.runAfter([&c = client] {
    c.conn()->send("ok");
//    c.conn()->shutdown();
  }, 1);
  loop.runAfter([&l = loop] {
    l.quit();
  }, 2);
  loop.loop();
}
