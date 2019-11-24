//
// Created by suun on 11/24/19.
//

#ifndef LOTTA_EVENT_LOOP_POOL_H
#define LOTTA_EVENT_LOOP_POOL_H

#include <memory>
#include <vector>

namespace lotta {

class EventLoop;
class EventLoopThread;

class EventLoopPool {
 public:
  explicit EventLoopPool(EventLoop *loop);
  ~EventLoopPool();

  void setThreadNum(unsigned n) { threadNum_ = n; }
  void start();
  EventLoop *getLoop();

 private:
  EventLoop *base_;
  unsigned idx_;
  unsigned threadNum_;
  bool started_;

  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop *> loops_;
};

}

#endif //LOTTA_EVENT_LOOP_POOL_H
