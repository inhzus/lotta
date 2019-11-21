//
// Created by suun on 11/9/19.
//

#ifndef LOTTA_POLLER_H
#define LOTTA_POLLER_H

#include "lotta/event_loop.h"
#include "lotta/utils/noncopyable.h"
#include <map>
#include <memory>
#include <vector>

namespace lotta {

class Channel;

class Poller : utils::noncopyable {
 public:
  explicit Poller(EventLoop *loop);
  virtual ~Poller();
  virtual void poll(int timeout, std::vector<Channel *> &channels) = 0;
  virtual void updateChannel(Channel *channel) = 0;
  virtual void removeChannel(Channel *channel) = 0;
  static std::unique_ptr<Poller> get(EventLoop *loop);

 protected:
  std::map<int, Channel *> channels_;
  EventLoop *loop_;
};

}

#endif //LOTTA_POLLER_H
