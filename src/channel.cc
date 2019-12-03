//
// Created by suun on 11/8/19.
//

#include "lotta/channel.h"
#include "lotta/event_loop.h"
#include "lotta/utils/logging.h"
#include <sstream>
#include <poll.h>

namespace lotta {

const unsigned kEmptyEvent = 0;
const unsigned kReadPollEvent = static_cast<unsigned>(POLLIN)
    | static_cast<unsigned>(POLLPRI)
    | static_cast<unsigned>(POLLRDHUP);
const unsigned kWritePollEvent = static_cast<unsigned>(POLLOUT);
const unsigned kErrorPollEvent = static_cast<unsigned>(POLLERR)
    | static_cast<unsigned>(POLLNVAL);

std::string innerEventsString(unsigned e) {
  std::stringstream ss;
  if (e & POLLIN) { ss << "IN "; } // NOLINT(hicpp-signed-bitwise)
  if (e & POLLPRI) { ss << "PRI "; } // NOLINT(hicpp-signed-bitwise)
  if (e & POLLRDHUP) { ss << "RDHUP "; } // NOLINT(hicpp-signed-bitwise)
  if (e & POLLOUT) { ss << "OUT "; } // NOLINT(hicpp-signed-bitwise)
  if (e & POLLERR) { ss << "ERR "; } // NOLINT(hicpp-signed-bitwise)
  if (e & POLLNVAL) { ss << "NVAL "; } // NOLINT(hicpp-signed-bitwise)
  std::string ret(ss.str());
  if (ret.empty()) {
    ret = "NULL";
  }
  return ret;
}

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(),
      revents_(),
      idxPoll_(-1) {}

void Channel::handleEvent() {
  if (revents_ & POLLNVAL) { // NOLINT(hicpp-signed-bitwise)
    SPDLOG_WARN("invalid polling request");
  }
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN) // NOLINT
      && closeCallback_) {
    closeCallback_();
  }
  if (revents_ & kErrorPollEvent && errorCallback_) {
    errorCallback_();
  }
  if (revents_ & kReadPollEvent && readCallback_) {
    readCallback_();
  }
  if (revents_ & kWritePollEvent && writeCallback_) {
    writeCallback_();
  }
}
void Channel::update() {
  loop_->updateChannel(this);
}
void Channel::remove() {
  assert(isEmptyEvent());
  loop_->removeChannel(this);
}

void Channel::setReadCallback(Channel::Callback readCallback) {
  readCallback_ = std::move(readCallback);
}
void Channel::setWriteCallback(Channel::Callback writeCallback) {
  writeCallback_ = std::move(writeCallback);
}
void Channel::setCloseCallback(lotta::Channel::Callback closeCallback) {
  closeCallback_ = std::move(closeCallback);
}
void Channel::setErrorCallback(Channel::Callback errorCallback) {
  errorCallback_ = std::move(errorCallback);
}

void Channel::enableReading() {
  events_ |= kReadPollEvent;
  update();
}
void Channel::disableReading() {
  events_ &= ~kReadPollEvent;
  update();
}
void Channel::enableWriting() {
  events_ |= kWritePollEvent;
  update();
}
void Channel::disableWriting() {
  events_ &= ~kWritePollEvent;
  update();
}

void Channel::disableEvents() {
  events_ = kEmptyEvent;
  update();
}

int Channel::fd() const {
  return fd_;
}
unsigned Channel::events() const {
  return events_;
}
void Channel::setRevents(unsigned revents) {
  revents_ = revents;
}
int Channel::idxPoll() const {
  return idxPoll_;
}
void Channel::setIdxPoll(int idxPoll) {
  idxPoll_ = idxPoll;
}
bool Channel::isEmptyEvent() const {
  return events_ == kEmptyEvent;
}
bool Channel::isReading() const {
  return events_ & kReadPollEvent;
}
bool Channel::isWriting() const {
  return events_ & kWritePollEvent;
}
std::string Channel::eventsString() const {
  return innerEventsString(events_);
}
std::string Channel::reventsString() const {
  return innerEventsString(revents_);
}
}
