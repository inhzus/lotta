//
// Created by suun on 11/10/19.
//

#include "lotta/timer_queue.h"
#include "lotta/channel.h"
#include "lotta/socket.h"
#include "lotta/utils/logging.h"
#include <sys/timerfd.h>

namespace lotta {

int createTimerFd() {
  int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (fd < 0) {
    SPDLOG_CRITICAL("timer fd fails to initialize");
  }
  return fd;
}

void readTimerFd(int fd) {
  int64_t tmp;
  ssize_t n = socket::read(fd, &tmp, sizeof(tmp));
  SPDLOG_TRACE("read timer fd {} events", tmp);
}

void resetTimerFd(int fd, TimerQueue::time_point point) {
  auto duration = point - TimerQueue::clock::now();
  auto nanoseconds = std::chrono::duration_cast<
      std::chrono::nanoseconds>(duration).count();
  itimerspec spec{
      {},
      {nanoseconds / 1'000'000'000,
       nanoseconds % 1'000'000'000}};
  if (::timerfd_settime(fd, 0, &spec, nullptr)) {
    SPDLOG_CRITICAL("fail to set timer fd");
  }
}

std::pair<TimerQueue::TimerMultiMap::iterator, bool>
findTimer(TimerQueue::TimerMultiMap &timers,
          const TimerQueue::TimerEntry &entry) {
  auto range = timers.equal_range(entry.first);
  auto it = std::find_if(
      range.first, range.second,
      [&entry = entry](const TimerQueue::TimerEntry &item) -> bool {
        return entry.second == item.second;
      });
  bool isFound = (range.second != it);
  return std::pair<TimerQueue::TimerMultiMap::iterator, bool>(it, isFound);
}

TimerQueue::TimerQueue(lotta::EventLoop *loop) :
    fd_(createTimerFd()),
    channel_(std::make_unique<Channel>(loop, fd_)),
    loop_(loop),
    doingTimerCallback_(false) {
  channel_->setReadCallback(std::bind(&TimerQueue::handleTimer, this));
  channel_->enableReading();
}

TimerQueue::~TimerQueue() {
  channel_->disableEvents();
  socket::close(fd_);
}

std::weak_ptr<Timer> TimerQueue::addTimer(
    TimerQueue::Callback callback,
    TimerQueue::time_point when,
    double interval) {
  std::shared_ptr<Timer> timer = std::make_shared<Timer>(
      std::move(callback), when, interval);
  loop_->exec(std::bind(&TimerQueue::addTask, this, timer));
  return timer;
}

void TimerQueue::cancel(const std::weak_ptr<Timer> &weakTimer) {
  std::shared_ptr<Timer> timer = weakTimer.lock();
  assert(timer != nullptr);
  loop_->exec(std::bind(&TimerQueue::cancelTask, this, timer));
}

void TimerQueue::handleTimer() {
  loop_->assertTheSameThread();
  time_point now = clock::now();
  readTimerFd(fd_);
  auto expired = expireTimers(now);

  doingTimerCallback_ = true;
  cancelledTimers_.clear();
  for (const auto &item : expired) {
    item.second->run();
  }
  doingTimerCallback_ = false;

  resetTimers(now, expired);
}

std::vector<TimerQueue::TimerEntry>
TimerQueue::expireTimers(TimerQueue::time_point now) {
  auto expiredEnd = timers_.lower_bound(now);
  assert(expiredEnd == timers_.end() || now < expiredEnd->first);
  std::vector<std::pair<time_point, std::shared_ptr<Timer>>> expired(
      timers_.begin(), expiredEnd);
  timers_.erase(timers_.begin(), expiredEnd);
  return expired;
}

void TimerQueue::resetTimers(
    TimerQueue::time_point now, const std::vector<TimerEntry> &expired) {
  for (const TimerEntry &entry : expired) {
    auto[it, isCancelledWhenCallback] = findTimer(cancelledTimers_, entry);
    if (entry.second->isRepeat() && !isCancelledWhenCallback) {
      entry.second->restart(now);
      pushTimerToMap(entry.second);
    }
  }
  if (!timers_.empty() && timers_.begin()->first > now) {
    resetTimerFd(fd_, timers_.begin()->first);
  }
}

void TimerQueue::addTask(const std::shared_ptr<Timer> &timer) {
  loop_->assertTheSameThread();
  time_point when = timer->getWhen();
  bool pushedToFirst = pushTimerToMap(timer);
  if (pushedToFirst) {
    resetTimerFd(fd_, when);
  }
}

void TimerQueue::cancelTask(const std::shared_ptr<Timer> &timer) {
  loop_->assertTheSameThread();
  auto[it, isFound] = findTimer(
      timers_, TimerEntry(timer->getWhen(), timer));
  if (isFound) {
    timers_.erase(it);
  } else if (doingTimerCallback_) {
    cancelledTimers_.insert(TimerEntry(timer->getWhen(), timer));
  }
}

bool TimerQueue::pushTimerToMap(const std::shared_ptr<Timer> &timer) {
  bool pushedToFirst = (
      timers_.empty() || timer->getWhen() < timers_.begin()->first);
  timers_.insert(TimerEntry(timer->getWhen(), timer));
  return pushedToFirst;
}

}
