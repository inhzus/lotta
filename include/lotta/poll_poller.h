//
// Created by suun on 11/9/19.
//

#ifndef LOTTA_POLL_POLLER_H
#define LOTTA_POLL_POLLER_H

#include "lotta/poller.h"

struct pollfd;

namespace lotta {

class PollPoller : public Poller {
 public:
  explicit PollPoller(EventLoop *loop);
  void poll(int timeout, std::vector<Channel *> &activeChannels) override;
  void updateChannel(Channel *channel) override;
  void removeChannel(Channel *channel) override;
 private:
  std::vector<pollfd> pollFds_;
};

}

#endif //LOTTA_POLL_POLLER_H
