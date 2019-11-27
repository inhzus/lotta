//
// Created by suun on 11/26/19.
//

#include "lotta/http/server.h"
#include "lotta/net_addr.h"
#include "lotta/tcp_connection.h"
#include "lotta/tcp_server.h"
#include "lotta/http/context.h"
#include "lotta/http/exceptions.h"
#include "lotta/http/request.h"
#include "lotta/http/response.h"
#include "lotta/utils/logging.h"

namespace lotta::http {

Server::Server(
    lotta::EventLoop *loop,
    const lotta::NetAddr &addr,
    std::string name = std::string()) :
    tcpServer_(std::make_unique<TcpServer>(loop, addr, std::move(name))),
    routes_() {
  using namespace std::placeholders;
  tcpServer_->setConnCallback(std::bind(&Server::connCallback, _1));
  tcpServer_->setMsgCallback(std::bind(&Server::msgCallback, this, _1, _2));
  tcpServer_->setCloseCallback(std::bind(&Server::closeCallback, _1));
}

Server::~Server() = default;

void Server::run() {
  SPDLOG_TRACE("http server runs at {}", tcpServer_->addr());
  tcpServer_->start();
}

void Server::route(Method method, const std::string &path, RouteFunc f) {
  routes_[std::make_pair(method, path)] = std::move(f);
}

void Server::connCallback(const TcpConnPtr &) {
  SPDLOG_TRACE("server connection established");
}

void Server::msgCallback(const TcpConnPtr &conn, Buffer *buf) {
  SPDLOG_TRACE("server received message");
  Request request(buf);
  Context context;

  try {
    auto found = routes_.find(std::make_pair(request.method(), request.path()));
    if (found == routes_.end()) {
      throw NotFoundException();
    }
    context.setRequest(request);
    context.setResponse(Response());
    found->second(&context);
  } catch (HttpException &e) {
    SPDLOG_WARN("during ");
  }
}

void Server::closeCallback(const TcpConnPtr &) {
  SPDLOG_TRACE("server connection closed");
}

}
