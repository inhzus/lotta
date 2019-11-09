//
// Created by suun on 11/9/19.
//

#include "lotta/poll_poller.h"
#include "lotta/channel.h"
#include "lotta/utils/logging.h"
#include <cassert>
#include <poll.h>
namespace lotta {


PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {}

void PollPoller::poll(
    int timeout, std::vector<Channel *> &activeChannels) {
  int num = ::poll(&*pollFds_.begin(), pollFds_.size(), timeout);
  if (num < 0) {
    logger->error("poll returns negative number");
  } else if (num == 0) {
    logger->trace("poll timeout");
  } else {
    for (const pollfd &pfd : pollFds_) {
      if (pfd.revents != 0) {
        auto found = channels_.find(pfd.fd);
        assert(found != channels_.end());
        Channel *channel = found->second;
        assert(pfd.fd == channel->getFd());
        channel->setRevents(pfd.revents);
        activeChannels.push_back(channel);
        if (--num <= 0) break;
      }
    }
  }
}

void PollPoller::updateChannel(Channel *channel) {
  loop_->assertThreadLoop();
  logger->trace("fd: {}, events: {}", channel->getFd(), channel->getEvents());
  if (channel->getIdxPoll() < 0) {
    assert(channels_.find(channel->getFd()) == channels_.end());
    pollfd pfd{
        channel->getFd(), static_cast<short>(channel->getEvents()), 0};
    pollFds_.push_back(pfd);
    channel->setIdxPoll(static_cast<int> (pollFds_.size()) - 1);
    channels_[pfd.fd] = channel;
  } else {
    assert(channels_.find(channel->getFd()) != channels_.end());
    assert(channels_[channel->getFd()] == channel);
    int idx = channel->getIdxPoll();
    assert(0 <= idx && idx < static_cast<int>(pollFds_.size()));
    pollfd &pfd = pollFds_[idx];
    assert(pfd.fd == channel->getFd() || pfd.fd == -1);
    pfd.events = static_cast<short>(channel->getEvents());
    pfd.revents = 0;
    if (channel->isEmptyEvent()) {
      pfd.fd = -1;
    }
  }
}
}
