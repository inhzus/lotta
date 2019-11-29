//
// Created by suun on 11/28/19.
//

#include "lotta/http/server.h"
#include "lotta/http/context.h"
#include "lotta/http/request.h"
#include "lotta/http/response.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/utils/logging.h"
#include <gtest/gtest.h>

using namespace lotta;
using namespace lotta::http;

TEST(HttpServer, default_) {
  spdlog::set_level(spdlog::level::err);
  NetAddr addr(8001);
  EventLoop loop;
  Server server(&loop, addr, "server");
  server.setThreadNum(8);
  server.route(Method::GET, "/", [](Context *context) {
    context->response()->setBody(context->request()->body());
  });
  server.run();
  loop.loop();
}
