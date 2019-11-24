//
// Created by suun on 11/24/19.
//

#include "lotta/event_loop_thread.h"
#include "lotta/event_loop.h"
#include "lotta/utils/logging.h"
#include <functional>
namespace lotta {

EventLoopThread::EventLoopThread() :
    loop_(nullptr),
    thread_(),
    mtx_(),
    cond_() {}

EventLoopThread::~EventLoopThread() {
  if (loop_ != nullptr) {
    loop_->quit();
  }
  if (thread_.joinable()) {
    thread_.join();
  }
}

EventLoop *EventLoopThread::start() {
  thread_ = std::thread(std::bind(&EventLoopThread::threadTask, this));
  std::unique_lock<std::mutex> lock(mtx_);
//  while (loop_ == nullptr) {
//    cond_.wait(lock);
//  }
  cond_.wait(lock, [this] { return this->loop_ != nullptr; });
  return loop_;
}

void EventLoopThread::threadTask() {
  EventLoop loop;
  {
    std::lock_guard<std::mutex> guard(mtx_);
    loop_ = &loop;
  }
  cond_.notify_one();
  loop.loop();

  std::lock_guard<std::mutex> guard(mtx_);
  loop_ = nullptr;
}

}
