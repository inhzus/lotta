//
// Created by suun on 11/14/19.
//

#include "lotta/acceptor.h"
#include "lotta/channel.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/socket.h"
#include "lotta/utils/logging.h"
#include <cassert>
#include <cerrno>

namespace lotta {

Acceptor::Acceptor(
    EventLoop *loop, const NetAddr &addr,
    Acceptor::ConnCallback connCallback) :
    loop_(loop),
    idleFd_(socket::open("/dev/null", O_RDONLY | O_CLOEXEC)), // NOLINT
    socket_(std::make_unique<Socket>(socket::socket(addr.family()))),
    channel_(std::make_unique<Channel>(loop, socket_->fd())),
    connCallback_(std::move(connCallback)) {
  assert(idleFd_ >= 0);
  socket_->setReuseAddr(true);
  socket_->setReusePort(true);
  SPDLOG_INFO("bind on {}", addr.addr());
  socket_->bind(addr);
  channel_->setReadCallback(std::bind(&Acceptor::handleAccept, this));
}
Acceptor::~Acceptor() {
  channel_->disableEvents();
  socket::close(idleFd_);
}
void Acceptor::listen() {
  loop_->assertTheSameThread();
  socket_->listen();
  channel_->enableReading();
}
void Acceptor::setConnCallback(Acceptor::ConnCallback cb) {
  connCallback_ = std::move(cb);
}

void Acceptor::handleAccept() {
  loop_->assertTheSameThread();
  NetAddr peerAddr;
  int fd = socket_->accept(peerAddr);
  if (fd >= 0) {
    if (connCallback_) {
      connCallback_(fd, peerAddr);
    } else {
      socket::close(fd);
    }
  } else if (errno == ENFILE) {
    socket::close(idleFd_);
    fd = socket_->accept(peerAddr);
    socket::close(fd);
    idleFd_ = socket::open("/dev/null", O_RDONLY | O_CLOEXEC); // NOLINT
    assert(idleFd_ >= 0);
  }
}

}
