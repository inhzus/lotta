//
// Created by suun on 11/24/19.
//

#ifndef LOTTA_EVENT_LOOP_THREAD_H
#define LOTTA_EVENT_LOOP_THREAD_H

#include <condition_variable>
#include <thread>

namespace lotta {

class EventLoop;

class EventLoopThread {
 public:
  EventLoopThread();
  ~EventLoopThread();

  EventLoop *start();

 private:
  void threadTask();

  EventLoop *loop_;
  std::thread thread_;
  std::condition_variable cond_;
  std::mutex mtx_;
};

}
#endif //LOTTA_EVENT_LOOP_THREAD_H
