//
// Created by suun on 11/18/19.
//

#include "lotta/tcp_server.h"
#include "lotta/acceptor.h"
#include "lotta/buffer.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/socket.h"
#include "lotta/tcp_connection.h"
#include "lotta/utils/logging.h"

namespace lotta {

void defaultConnCallback(const TcpServer::ConnPtr &) {
  SPDLOG_TRACE("connection established");
}

void defaultMsgCallback(const TcpServer::ConnPtr &, Buffer *buf) {
  Slice slice = buf->retrieve();
  SPDLOG_TRACE("received: {}", slice);
}

void defaultCloseCallback(const TcpServer::ConnPtr &) {
  SPDLOG_TRACE("connection closed");
}

TcpServer::TcpServer(
    EventLoop *loop,
    const NetAddr &addr,
    std::string name) :
    loop_(loop),
    acceptor_(std::make_unique<Acceptor>(loop, addr)),
    connCallback_(defaultConnCallback),
    msgCallback_(defaultMsgCallback),
    closeCallback_(defaultCloseCallback),
    connections_(),
    connIdx_(),
    name_(std::move(name)),
    started_(false) {
  using namespace std::placeholders;
  acceptor_->setConnCallback(
      std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
  SPDLOG_TRACE("tcp server {} destructing", name_);
  for (auto &pair : connections_) {
    auto conn = pair.second;
    loop_->exec(std::bind(&TcpConnection::handleClose, conn));
  }
}

void TcpServer::start() {
  if (started_) {
    SPDLOG_WARN("server started");
    return;
  }
  started_ = true;
  loop_->exec(std::bind(&Acceptor::listen, acceptor_.get()));
}

void TcpServer::newConnection(int fd, const lotta::NetAddr &addr) {
  loop_->assertTheSameThread();
  NetAddr localAddr(socket::getSockName(fd));
  std::string connName = fmt::format(
      "{}-{}#{}", name_, localAddr.addr(), connIdx_++);
  SPDLOG_INFO("New connection {} from {}", connName, addr.addr());
  ConnPtr conn = std::make_shared<TcpConnection>(
      loop_, connName, fd, localAddr, addr);
  connections_[connName] = conn;
  conn->setConnCallback(connCallback_);
  conn->setMsgCallback(msgCallback_);
  conn->setCloseCallback(closeCallback_);
  conn->setRmConnCallback(std::bind(
      &TcpServer::removeConn, this, std::placeholders::_1));
  conn->connEstablished();
}

void TcpServer::removeConn(const TcpServer::ConnPtr &conn) {
  loop_->exec([this, &conn = conn]() {
    this->loop_->assertTheSameThread();
    SPDLOG_INFO("remove connection {}", conn->name());  // NOLINT
    [[maybe_unused]] auto n = this->connections_.erase(conn->name());
    assert(n == 1);
  });
}

}
