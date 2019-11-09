//
// Created by suun on 11/7/19.
//

#include "lotta/event_loop.h"
#include "lotta/channel.h"
#include "lotta/poller.h"
#include "lotta/utils/this_thread.h"
#include "lotta/utils/logging.h"

namespace lotta {

thread_local EventLoop *t_eventLoopThisThread = nullptr;
const int kPollTimeout = 5 * 1000;

EventLoop::EventLoop() :
    looping_(false),
    quit_(false),
    threadId_(utils::this_thread::id()),
    poller_(Poller::get(this)) {
  logger->trace("EventLoop {} created in thread {:x}",
                static_cast<void *>(this),
                threadId_);
  if (t_eventLoopThisThread) {
    logger->trace("another EventLoop {} exists",
                  (void *) t_eventLoopThisThread);
  } else {
    t_eventLoopThisThread = this;
  }
}

EventLoop::~EventLoop() {
  t_eventLoopThisThread = nullptr;
}

void EventLoop::run() {
  assertThreadLoop();
  looping_ = true;
  quit_ = false;
  logger->trace("EventLoop {} looping", static_cast<void *>(this));
  while (!quit_) {
    poller_->poll(kPollTimeout, activeChannels_);
    for (auto channel : activeChannels_) {
      channel->handleEvent();
    }
  }
  logger->trace("EventLoop {} stops", static_cast<void *>(this));
  looping_ = false;
}
void EventLoop::quit() {
  quit_ = true;
}

void EventLoop::updateChannel(Channel *channel) {
  poller_->updateChannel(channel);
}

void EventLoop::assertThreadLoop() const {
  if (!isThreadLoop()) {
    logger->trace("this is not specified run");
  }
}
bool EventLoop::isThreadLoop() const {
  return t_eventLoopThisThread == this;
}

}
