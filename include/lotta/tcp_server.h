//
// Created by suun on 11/18/19.
//

#ifndef LOTTA_TCP_SERVER_H
#define LOTTA_TCP_SERVER_H

#include "lotta/utils/noncopyable.h"
#include <functional>
#include <map>
#include <memory>

namespace lotta {

class Acceptor;
class Buffer;
class EventLoop;
class NetAddr;
class TcpConnection;

class TcpServer : utils::noncopyable {
 public:
  using ConnPtr = std::shared_ptr<TcpConnection>;
  using ConnCallback = std::function<
      void(const ConnPtr &)>;
  using MsgCallback = std::function<
      void(const ConnPtr &, Buffer *)>;

  TcpServer(EventLoop *loop, const NetAddr &addr, std::string name);

 private:
  void newConnection(int fd, const NetAddr &addr);

  using ConnMap = std::map<std::string, ConnPtr>;

  EventLoop *loop_;
  std::unique_ptr<Acceptor> acceptor_;
  ConnCallback connCallback_;
  MsgCallback msgCallback_;
  ConnMap connections_;

  int connIdx_;
  std::string name_;
};

}

#endif //LOTTA_TCP_SERVER_H
