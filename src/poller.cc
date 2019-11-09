//
// Created by suun on 11/9/19.
//

#include "lotta/poller.h"
#include "lotta/poll_poller.h"

namespace lotta {

Poller::Poller(EventLoop *loop)
    : loop_(loop) {}
Poller::~Poller() = default;

std::unique_ptr<Poller> Poller::get(EventLoop *loop) {
  return std::unique_ptr<Poller>(new PollPoller(loop));
}

}
