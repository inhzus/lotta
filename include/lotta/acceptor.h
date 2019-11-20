//
// Created by suun on 11/14/19.
//

#ifndef LOTTA_ACCEPTOR_H
#define LOTTA_ACCEPTOR_H

#include "lotta/utils/noncopyable.h"
#include <functional>
#include <memory>

namespace lotta {

class Channel;
class EventLoop;
class NetAddr;
class Socket;

class Acceptor : utils::noncopyable {
 public:
  using ConnCallback = std::function<void(int, const NetAddr &)>;

  Acceptor(EventLoop *loop, const NetAddr &addr,
           ConnCallback connCallback = nullptr);
  ~Acceptor();

  void listen();
  void setConnCallback(ConnCallback);

 private:

  void handleAccept();

  EventLoop *loop_;
  int idleFd_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  ConnCallback connCallback_;
};

}

#endif //LOTTA_ACCEPTOR_H
