//
// Created by suun on 11/18/19.
//

#include "lotta/tcp_server.h"
#include "lotta/acceptor.h"
#include "lotta/buffer.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/socket.h"
#include "lotta/utils/logging.h"

namespace lotta {

void defaultConnCallback(const TcpServer::ConnPtr &) {
  SPDLOG_TRACE("connection");
}

void defaultMsgCallback(const TcpServer::ConnPtr &, Buffer *buf) {
  buf->retrieve();
}

TcpServer::TcpServer(
    EventLoop *loop,
    const NetAddr &addr,
    std::string name) :
    loop_(loop),
    acceptor_(std::make_unique<Acceptor>(loop, addr)),
    connCallback_(defaultConnCallback),
    msgCallback_(defaultMsgCallback),
    connections_(),
    connIdx_(),
    name_(std::move(name)) {
  using namespace std::placeholders;
  acceptor_->setConnCallback(
      std::bind(&TcpServer::newConnection, this, _1, _2));
}

void TcpServer::newConnection(int fd, const lotta::NetAddr &addr) {
  loop_->assertTheSameThread();
  NetAddr localAddr(socket::getSockName(fd));
  std::string connName = fmt::format(
      "{}-{}#{}", name_, localAddr.addr(), connIdx_++);
  SPDLOG_INFO("New connection {} from {}", connName, addr.addr());

}

}
