//
// Created by suun on 11/7/19.
//

#include "lotta/event_loop.h"
#include "lotta/channel.h"
#include "lotta/poller.h"
#include "lotta/socket.h"
#include "lotta/utils/this_thread.h"
#include "lotta/utils/logging.h"
#include <sys/eventfd.h>

namespace lotta {

thread_local EventLoop *t_eventLoopThisThread = nullptr;
const int kPollTimeout = 5 * 1000;

int createEventFd() {
  int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (fd < 0) {
    logger()->critical("event fd fails to initialize");
  }
  return fd;
}

EventLoop::EventLoop() :
    looping_(false),
    quit_(false),
    threadId_(utils::this_thread::id()),
    poller_(Poller::get(this)),
    wakeupFd_(createEventFd()),
    wakeChannel_(std::make_unique<Channel>(this, wakeupFd_)),
    doingFuncQueue_(false) {
  logger()->trace("EventLoop {} created in thread {:x}",
                  static_cast<void *>(this),
                  threadId_);
  if (t_eventLoopThisThread) {
    logger()->trace("another EventLoop {} exists",
                    (void *) t_eventLoopThisThread);
  } else {
    t_eventLoopThisThread = this;
  }
  wakeChannel_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
  wakeChannel_->enableReading();
}

EventLoop::~EventLoop() {
  t_eventLoopThisThread = nullptr;
  wakeChannel_->disableEvents();
  socket::close(wakeupFd_);
}

void EventLoop::loop() {
  assertTheSameThread();
  looping_ = true;
  quit_ = false;
  logger()->trace("EventLoop {} looping", static_cast<void *>(this));
  while (!quit_) {
    activeChannels_.clear();
    poller_->poll(kPollTimeout, activeChannels_);
    for (auto channel : activeChannels_) {
      channel->handleEvent();
    }
    doFuncQueue();
  }
  logger()->trace("EventLoop {} stops", static_cast<void *>(this));
  looping_ = false;
}
void EventLoop::quit() {
  quit_ = true;
  if (!isTheSameThread()) {
    wakeup();
  }
}

void EventLoop::updateChannel(Channel *channel) {
  poller_->updateChannel(channel);
}

void EventLoop::exec(EventLoop::Function f) {
  if (isTheSameThread()) {
    f();
  } else {
    pushQueue(std::move(f));
  }
}

void EventLoop::pushQueue(EventLoop::Function f) {
  {
    std::lock_guard guard(funcQueueMtx_);
    funcQueue_.push_back(std::move(f));
  }
  // iff in the same thread & not doing funcQueue (aka activeChannel callbacks)
  if (!isTheSameThread() | doingFuncQueue_) {
    wakeup();
  }
}

void EventLoop::doFuncQueue() {
  std::vector<Function> functions;
  doingFuncQueue_ = true;
  {
    std::lock_guard guard(funcQueueMtx_);
    functions.swap(funcQueue_);
  }
  for (Function &func : functions) {
    func();
  }
  doingFuncQueue_ = false;
}

void EventLoop::assertTheSameThread() const {
  if (!isTheSameThread()) {
    logger()->critical("not in the same thread of EventLoop");
  }
}
bool EventLoop::isTheSameThread() const {
  return t_eventLoopThisThread == this;
}

void EventLoop::wakeup() {
  int tmp = 1;
  auto n = socket::write(wakeupFd_, &tmp, sizeof(tmp));
  if (n != sizeof(tmp)) {
    logger()->error("fail to wakeup");
  }
}

void EventLoop::handleWakeup() {
  int tmp;
  auto n = socket::read(wakeupFd_, &tmp, sizeof(tmp));
  if (n != sizeof(n)) {
    logger()->error("fail to handle wakeup");
  }
}

}
