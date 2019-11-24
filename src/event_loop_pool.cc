//
// Created by suun on 11/24/19.
//

#include "lotta/event_loop_pool.h"
#include "lotta/event_loop.h"
#include "lotta/event_loop_thread.h"
#include <cassert>

namespace lotta {

EventLoopPool::EventLoopPool(EventLoop *loop) :
    base_(loop), idx_(0), threadNum_(0), started_(false),
    threads_(), loops_() {}

EventLoopPool::~EventLoopPool() = default;

void EventLoopPool::start() {
  base_->assertTheSameThread();
  assert(!started_);
  for (unsigned i = 0; i != threadNum_; i++) {
    auto thread = std::make_unique<EventLoopThread>();
    loops_.push_back(thread->start());
    threads_.push_back(std::move(thread));
  }
  started_ = true;
}

EventLoop *EventLoopPool::getLoop() {
  base_->assertTheSameThread();
  assert(started_);
  assert(threadNum_ == loops_.size());
  if (threadNum_ == 0) {
    return base_;
  }
  EventLoop *loop = loops_[idx_];
  idx_ = (idx_ + 1) % threadNum_;
  return loop;
}

}
