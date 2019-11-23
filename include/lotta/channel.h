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
  void remove();

  void setReadCallback(Callback);
  void setWriteCallback(Callback);
  void setCloseCallback(Callback);
  void setErrorCallback(Callback);

  void enableReading();
  void disableReading();
  void enableWriting();
  void disableWriting();
  void disableEvents();

  void setIdxPoll(int);
  void setRevents(unsigned);

  [[nodiscard]] unsigned events() const;
  [[nodiscard]] int fd() const;
  [[nodiscard]] int idxPoll() const;

  [[nodiscard]] bool isEmptyEvent() const;
  [[nodiscard]] bool isReading() const;
  [[nodiscard]] bool isWriting() const;

  [[nodiscard]] std::string eventsString() const;
  [[nodiscard]] std::string reventsString() const;
 private:
  EventLoop *loop_;
  const int fd_;
  unsigned events_;
  unsigned revents_;
  int idxPoll_;

  Callback readCallback_;
  Callback writeCallback_;
  Callback closeCallback_;
  Callback errorCallback_;
};

}

#endif //LOTTA_CHANNEL_H
