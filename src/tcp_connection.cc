//
// Created by suun on 11/21/19.
//

#include "lotta/tcp_connection.h"
#include "lotta/channel.h"
#include "lotta/event_loop.h"
#include "lotta/socket.h"
#include "lotta/utils/logging.h"
#include <cstring>

namespace lotta {

TcpConnection::TcpConnection(
    EventLoop *loop,
    std::string name,
    int fd,
    NetAddr localAddr,
    NetAddr peerAddr) :
    loop_(loop),
    name_(std::move(name)),
    state_(TcpConnection::State::Connecting),
    channel_(std::make_unique<Channel>(loop, fd)),
    socket_(std::make_unique<Socket>(fd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    inBuf(),
    outBuf() {
  channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
  channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
  socket_->setKeepAlive(true);
}
TcpConnection::~TcpConnection() {
  SPDLOG_TRACE("connection {} destructed", name_);
}

void TcpConnection::handleRead() {
  loop_->assertTheSameThread();
  ssize_t n = inBuf.readFrom(channel_->fd());
  if (n == 0) {
    handleClose();
  } else if (n < 0) {
    SPDLOG_WARN("handle read error as above");
  } else {
    msgCallback_(shared_from_this(), &inBuf);
  }
}

void TcpConnection::handleWrite() {
  loop_->assertTheSameThread();
  if (!channel_->isWriting()) {
    SPDLOG_INFO("fd {} down, no more writing", channel_->fd());
    return;
  }
  ssize_t n = socket::write(channel_->fd(), outBuf.peek(), outBuf.readable());
  if (n <= 0) {
    SPDLOG_WARN("write fail {}", errno);
    return;
  }
  outBuf.retrieve(n);
  if (outBuf.readable() == 0) {
    channel_->disableWriting();
    // TODO queue in loop: write complete callback
  }
}

void TcpConnection::handleClose() {
  loop_->assertTheSameThread();
  SPDLOG_TRACE("connection close");
  state_ = State::Disconnected;
  channel_->disableEvents();
  channel_->remove();
  closeCallback_(shared_from_this());
  rmConnCallback_(shared_from_this());
}

void TcpConnection::handleError() {
  loop_->assertTheSameThread();
  int err = socket::getSocketErrno(channel_->fd());
  SPDLOG_WARN("connection {} error {}", name_, std::strerror(err));
}

void TcpConnection::connEstablished() {
  loop_->assertTheSameThread();
  assert(state_ == State::Connecting);
  state_ = State::Connected;
  channel_->enableReading();
  connCallback_(shared_from_this());
}

void TcpConnection::setConnCallback(TcpConnection::ConnCallback cb) {
  connCallback_ = std::move(cb);
}
void TcpConnection::setMsgCallback(TcpConnection::MsgCallback cb) {
  msgCallback_ = std::move(cb);
}
void TcpConnection::setCloseCallback(TcpConnection::CloseCallback cb) {
  closeCallback_ = std::move(cb);
}
void TcpConnection::setRmConnCallback(TcpConnection::RmConnCallback cb) {
  rmConnCallback_ = std::move(cb);
}

}
