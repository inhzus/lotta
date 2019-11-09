//
// Created by suun on 11/8/19.
//

#ifndef LOTTA_CHANNEL_H
#define LOTTA_CHANNEL_H

#include "lotta/utils/noncopyable.h"
#include <functional>
#include <memory>

namespace lotta {

class EventLoop;

class Channel : utils::noncopyable {
 public:
  using Callback = std::function<void()>;
  Channel(EventLoop *loop, int fd);
  void handleEvent();
  void update();

  void setReadCallback(Callback);
  void setWriteCallback(Callback);
  void setErrorCallback(Callback);

  void enableReading();
  void disableReading();
  void enableWriting();
  void disableWriting();
  void setIdxPoll(int);
  void setRevents(unsigned);

  [[nodiscard]] unsigned getEvents() const;
  [[nodiscard]] int getFd() const;
  [[nodiscard]] int getIdxPoll() const;

  [[nodiscard]] bool isEmptyEvent() const;
 private:
  EventLoop *loop_;
  const int fd_;
  unsigned events_;
  unsigned revents_;
  int idxPoll_;

  Callback readCallback_;
  Callback writeCallback_;
  Callback errorCallback_;
};

}

#endif //LOTTA_CHANNEL_H
