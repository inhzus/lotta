//
// Created by suun on 11/7/19.
//

#include "lotta/event_loop.h"
#include "lotta/channel.h"
#include "lotta/poller.h"
#include "lotta/socket.h"
#include "lotta/timer.h"
#include "lotta/timer_queue.h"
#include "lotta/utils/this_thread.h"
#include "lotta/utils/logging.h"
#include <sys/eventfd.h>

namespace lotta {

thread_local EventLoop *t_eventLoopThisThread = nullptr;
const int kPollTimeout = 5 * 1000;

int createEventFd() {
  int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (fd < 0) {
    SPDLOG_CRITICAL("event fd fails to initialize");
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
    doingFuncQueue_(false),
    timerQueue_(std::make_unique<TimerQueue>(this)) {
  SPDLOG_TRACE("EventLoop {} created in thread {}",
               static_cast<void *>(this), threadId_);
  if (t_eventLoopThisThread) {
    SPDLOG_TRACE("another EventLoop {} exists", (void *) t_eventLoopThisThread);
  } else {
    t_eventLoopThisThread = this;
  }
  wakeChannel_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
  wakeChannel_->enableReading();
}

EventLoop::~EventLoop() {
  t_eventLoopThisThread = nullptr;
  wakeChannel_->disableEvents();
  wakeChannel_->remove();
  socket::close(wakeupFd_);
}

void EventLoop::loop() {
  assertTheSameThread();
  looping_ = true;
  quit_ = false;
  SPDLOG_TRACE("EventLoop {} looping", static_cast<void *>(this));
  while (!quit_) {
    activeChannels_.clear();
    poller_->poll(kPollTimeout, activeChannels_);
    for (auto channel : activeChannels_) {
      channel->handleEvent();
    }
    doFuncQueue();
  }
  SPDLOG_TRACE("EventLoop {} stops", static_cast<void *>(this));
  looping_ = false;
}
void EventLoop::quit() {
  quit_ = true;
  if (!isTheSameThread()) {
    wakeup();
  }
}

void EventLoop::updateChannel(Channel *channel) {
  assertTheSameThread();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
  assertTheSameThread();
  poller_->removeChannel(channel);
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

std::weak_ptr<Timer> EventLoop::runAfter(
    EventLoop::Function func, double interval) {
  Timer::time_point point = std::chrono::time_point_cast<Timer::duration>(
      Timer::clock::now() + std::chrono::duration<double>(interval));
  return timerQueue_->addTimer(std::move(func), point, 0);
}

std::weak_ptr<Timer> EventLoop::runEvery(
    EventLoop::Function func, double interval) {
  Timer::time_point point = std::chrono::time_point_cast<Timer::duration>(
      Timer::clock::now() + std::chrono::duration<double>(interval));
  return timerQueue_->addTimer(std::move(func), point, interval);
}

void EventLoop::cancel(const std::weak_ptr<Timer> &timer) {
  timerQueue_->cancel(timer);
}

void EventLoop::assertTheSameThread() const {
  if (!isTheSameThread()) {
    SPDLOG_CRITICAL("not in the same thread of EventLoop");
  }
}
bool EventLoop::isTheSameThread() const {
  return threadId_ == utils::this_thread::id();
}

void EventLoop::wakeup() {
  uint64_t tmp = 1;
  auto n = socket::write(wakeupFd_, &tmp, sizeof(tmp));
  SPDLOG_ERRNO_IF(n != sizeof(tmp));
}

void EventLoop::handleWakeup() {
  uint64_t tmp;
  auto n = socket::read(wakeupFd_, &tmp, sizeof(tmp));
  SPDLOG_ERRNO_IF(n != sizeof(tmp));
}

}
