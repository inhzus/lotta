//
// Created by suun on 12/2/19.
//

#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/http/request.h"
#include "lotta/http/response.h"
#include "lotta/http/server.h"
#include "lotta/utils/logging.h"

using namespace lotta;
using namespace lotta::http;

int main() {
  spdlog::set_level(spdlog::level::err);
  EventLoop loop;
  Server server(&loop, NetAddr(8001), "LottaServer");
  server.route(Method::GET, "/", [](const Request &req, Response &resp) {
    resp.setBody("Hello world!");
  });
  server.setThreadNum(8);
  server.run();
  loop.loop();
}
