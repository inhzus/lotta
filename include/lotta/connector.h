//
// Created by suun on 11/25/19.
//

#ifndef LOTTA_CONNECTOR_H
#define LOTTA_CONNECTOR_H

#include "lotta/utils/noncopyable.h"
#include <functional>
#include <memory>

namespace lotta {

class Channel;
class EventLoop;
class NetAddr;

class Connector : utils::noncopyable,
                  public std::enable_shared_from_this<Connector> {
 public:
  using ConnCallback = std::function<void(int)>;

  Connector(EventLoop *loop, const NetAddr &addr);
  ~Connector();

  void start();
  void setConnCallback(ConnCallback);

 private:
  enum class State {
    Connecting, Connected, Disconnected
  };
  static const unsigned kInitRetrySec;
  static const unsigned kMaxRetrySec;

  void startTask();

  void connected(int fd);
  void retry(int fd);

  void handleWrite();
  void handleError();

  int removeChannel();
  void defaultConnCallback(int fd);

  EventLoop *loop_;
  std::unique_ptr<NetAddr> addr_;
  std::unique_ptr<Channel> channel_;
  State state_;
  bool toQuit_;
  unsigned retryTimeoutSec_;
  ConnCallback connCallback_;
};

}

#endif //LOTTA_CONNECTOR_H
