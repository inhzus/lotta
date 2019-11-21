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
    SPDLOG_ERROR("poll returns negative number");
  } else if (num == 0) {
    SPDLOG_TRACE("poll timeout");
  } else {
    for (const pollfd &pfd : pollFds_) {
      if (pfd.revents != 0) {
        auto found = channels_.find(pfd.fd);
        assert(found != channels_.end());
        Channel *channel = found->second;
        assert(pfd.fd == channel->fd());
        channel->setRevents(pfd.revents);
        activeChannels.push_back(channel);
        if (--num <= 0) break;
      }
    }
  }
}

void PollPoller::updateChannel(Channel *channel) {
  loop_->assertTheSameThread();
  SPDLOG_TRACE("fd: {}, events: {}", channel->fd(), channel->eventsString());
  if (channel->idxPoll() < 0) {
    assert(channels_.find(channel->fd()) == channels_.end());
    pollfd pfd{
        channel->fd(), static_cast<short>(channel->events()), 0};
    pollFds_.push_back(pfd);
    channel->setIdxPoll(static_cast<int> (pollFds_.size()) - 1);
    channels_[pfd.fd] = channel;
  } else {
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    int idx = channel->idxPoll();
    assert(0 <= idx && idx < static_cast<int>(pollFds_.size()));
    pollfd &pfd = pollFds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if (channel->isEmptyEvent()) {
      pfd.fd = -1;
    }
  }
}
void PollPoller::removeChannel(Channel *channel) {
  loop_->assertTheSameThread();
  SPDLOG_TRACE("remove channel with fd {}", channel->fd());
  assert(channels_.find(channel->fd()) != channels_.end());
  assert(channels_[channel->fd()] == channel);
  assert(channel->isEmptyEvent());
  int idx = channel->idxPoll();
  assert(0 <= idx && idx < static_cast<int>(pollFds_.size()));
  assert(pollFds_[idx].fd == -1 && pollFds_[idx].events == channel->events());
  [[maybe_unused]] size_t n = channels_.erase(channel->fd());
  assert(n == 1);
  if (idx != pollFds_.size() - 1) {
    std::iter_swap(pollFds_.begin() + idx, pollFds_.end() - 1);
  }
  pollFds_.pop_back();
}
}
