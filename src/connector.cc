//
// Created by suun on 11/25/19.
//

#include "lotta/connector.h"
#include "lotta/channel.h"
#include "lotta/event_loop.h"
#include "lotta/net_addr.h"
#include "lotta/socket.h"
#include "lotta/utils/logging.h"
#include <cassert>
#include <cerrno>
#include <cstring>

namespace lotta {

const unsigned Connector::kInitRetrySec = 1;
const unsigned Connector::kMaxRetrySec = 30;

Connector::Connector(EventLoop *loop, const NetAddr &addr) :
    loop_(loop),
    addr_(std::make_unique<NetAddr>(addr)),
    channel_(),
    state_(State::Disconnected),
    toQuit_(false),
    retryTimeoutSec_(kInitRetrySec),
    connCallback_(std::bind(
        &Connector::defaultConnCallback, this, std::placeholders::_1)) {}

Connector::~Connector() {
  SPDLOG_TRACE("connector destructed");
  if (channel_ != nullptr) {
    channel_->disableEvents();
    channel_->remove();
    socket::close(channel_->fd());
  }
}

void Connector::start() {
  toQuit_ = false;
  loop_->exec(std::bind(&Connector::startTask, this));
}

void Connector::setConnCallback(Connector::ConnCallback cb) {
  connCallback_ = std::move(cb);
}

void Connector::startTask() {
  loop_->assertTheSameThread();
  assert(state_ == State::Disconnected);
  if (toQuit_) {
    SPDLOG_TRACE("connector stops");
    return;
  }
  int fd = socket::socket(addr_->family());
  SPDLOG_ERRNO_IF(fd < 0);
  int c = socket::connect(fd, addr_->addr_);
  switch (c == 0 ? 0 : errno) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN: {
      SPDLOG_ERRNO_L(TRACE, "async connect");
      connected(fd);
      break;
    }
    case EADDRNOTAVAIL:
    case EAGAIN:
    case ECONNREFUSED:
    case ENETUNREACH:
    case ETIMEDOUT: {
      retry(fd);
      break;
    }
    default: {
      SPDLOG_ERRNO();
      break;
    }
  }
}

void Connector::connected(int fd) {
  state_ = State::Connecting;
  assert(channel_ == nullptr);
  channel_ = std::make_unique<Channel>(loop_, fd);
  channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
  channel_->setErrorCallback(std::bind(&Connector::handleError, this));
  channel_->enableWriting();
}

void Connector::retry(int fd) {
  socket::close(fd);
  state_ = State::Disconnected;
  if (toQuit_) {
    SPDLOG_TRACE("disconnect before retry");
    return;
  }
  SPDLOG_INFO("retry connect to {} in {} sec", addr_->addr(), retryTimeoutSec_);
  loop_->runAfter(std::bind(&Connector::startTask, this), retryTimeoutSec_);
  retryTimeoutSec_ = std::min(retryTimeoutSec_ * 2, kMaxRetrySec);
}

void Connector::handleWrite() {
  if (state_ != State::Connecting) {
    SPDLOG_ERROR("state is {}", state_);
    return;
  }
  int fd = removeChannel();
  int err = socket::getSocketErrno(fd);
  if (err < 0) {
    SPDLOG_ERROR("fail({}) to connect, retry then", std::strerror(err));
    return retry(fd);
  }
  if (toQuit_) {
    socket::close(fd);
    SPDLOG_TRACE("disconnect after connection established");
    state_ = State::Disconnected;
    return;
  }
  state_ = State::Connected;
  SPDLOG_TRACE("connection established");
  if (connCallback_ != nullptr) {
    connCallback_(fd);
  }
}

void Connector::handleError() {
  if (state_ != State::Connecting) {
    SPDLOG_ERROR("state is {}", state_);
    return;
  }
  int fd = removeChannel();
  int err = socket::getSocketErrno(fd);
  SPDLOG_ERROR("fail{} when connecting", std::strerror(err));
  retry(fd);
}

int Connector::removeChannel() {
  int fd = channel_->fd();
  channel_->disableEvents();
  channel_->remove();
  loop_->pushQueue([&c = channel_] {
    c.reset();
  });
  return fd;
}

void Connector::defaultConnCallback(int fd) {
  channel_ = std::make_unique<Channel>(loop_, fd);
}

}
