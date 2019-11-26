//
// Created by suun on 11/26/19.
//

#include "lotta/tcp_client.h"

#include <utility>
#include "lotta/buffer.h"
#include "lotta/connector.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/socket.h"
#include "lotta/tcp_connection.h"
#include "lotta/utils/logging.h"

namespace lotta {

static void defaultConnCallback(const TcpClient::ConnPtr &conn) {
  SPDLOG_TRACE("client connected");
}
static void defaultMsgCallback(const TcpClient::ConnPtr &conn, Buffer *buf) {
  Slice slice = buf->retrieve();
  SPDLOG_TRACE("recv: {}", slice);
}
static void defaultCloseCallback(const TcpClient::ConnPtr &conn) {
  SPDLOG_TRACE("client disconnected");
}

TcpClient::TcpClient(EventLoop *loop, const NetAddr &addr, std::string name) :
    loop_(loop),
    connector_(std::make_unique<Connector>(loop, addr)),
    connCallback_(defaultConnCallback),
    msgCallback_(defaultMsgCallback),
    closeCallback_(defaultCloseCallback),
    mtx_(),
    connection_(),
    connIdx_(),
    name_(std::move(name)) {
  connector_->setConnCallback(std::bind(
      &TcpClient::newConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient() {
  SPDLOG_TRACE("client destructed");
}

void TcpClient::connect() {
  SPDLOG_TRACE("client connecting");
  connector_->start();
}

TcpClient::ConnPtr TcpClient::conn() {
  std::lock_guard<std::mutex> guard(mtx_);
  return connection_;
}

void TcpClient::newConnection(int fd) {
  loop_->assertTheSameThread();
  NetAddr localAddr(socket::getSockName(fd)),
      peerAddr(socket::getPeerName(fd));
  std::string connName = fmt::format(
      "{}-{}#{}", name_, localAddr.addr(), connIdx_++);
  ConnPtr conn = std::make_shared<TcpConnection>(
      loop_, connName, fd, localAddr, peerAddr);
  conn->setConnCallback(connCallback_);
  conn->setMsgCallback(msgCallback_);
  conn->setRmConnCallback(std::bind(
      &TcpClient::removeConn, this, std::placeholders::_1));
  conn->setCloseCallback(closeCallback_);
  loop_->exec(std::bind(&TcpConnection::connEstablished, conn));
  {
    std::lock_guard<std::mutex> guard(mtx_);
    connection_ = conn;
  }
}

void TcpClient::removeConn(const TcpClient::ConnPtr &conn) {
  loop_->exec([this, &conn = conn]() {
    this->loop_->assertTheSameThread();
    SPDLOG_INFO("remove connection {}", conn->name());  // NOLINT
    {
      std::lock_guard<std::mutex> guard(mtx_);
      this->connection_.reset();
    }
  });
}

void TcpClient::setConnCallback(TcpClient::ConnCallback cb) {
  connCallback_ = std::move(cb);
}
void TcpClient::setMsgCallback(TcpClient::MsgCallback cb) {
  msgCallback_ = std::move(cb);
}
void TcpClient::setCloseCallback(TcpClient::CloseCallback cb) {
  closeCallback_ = std::move(cb);
}

}
