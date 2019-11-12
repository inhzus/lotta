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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
std::string innerEventsString(unsigned e) {
  std::stringstream ss;
  if (e & POLLIN) { ss << "IN "; }
  if (e & POLLPRI) { ss << "PRI "; }
  if (e & POLLRDHUP) { ss << "RDHUP "; }
  if (e & POLLOUT) { ss << "OUT "; }
  if (e & POLLERR) { ss << "ERR "; }
  if (e & POLLNVAL) { ss << "NVAL "; }
  std::string ret(ss.str());
  if (ret.empty()) {
    ret = "NULL";
  }
  return ret;
}
#pragma clang diagnostic pop

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

void Channel::setReadCallback(Channel::Callback readCallback) {
  readCallback_ = std::move(readCallback);
}
void Channel::setWriteCallback(Channel::Callback writeCallback) {
  writeCallback_ = std::move(writeCallback);
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

int Channel::getFd() const {
  return fd_;
}
unsigned Channel::getEvents() const {
  return events_;
}
void Channel::setRevents(unsigned revents) {
  revents_ = revents;
}
int Channel::getIdxPoll() const {
  return idxPoll_;
}
void Channel::setIdxPoll(int idxPoll) {
  idxPoll_ = idxPoll;
}
bool Channel::isEmptyEvent() const {
  return events_ == kEmptyEvent;
}
std::string Channel::eventsString() const {
  return innerEventsString(events_);
}
std::string Channel::reventsString() const {
  return innerEventsString(revents_);
}
}
