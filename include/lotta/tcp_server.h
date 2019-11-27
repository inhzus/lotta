//
// Created by suun on 11/18/19.
//

#ifndef LOTTA_TCP_SERVER_H
#define LOTTA_TCP_SERVER_H

#include "lotta/utils/noncopyable.h"
#include <atomic>
#include <functional>
#include <map>
#include <memory>

namespace lotta {

class Acceptor;
class Buffer;
class EventLoop;
class EventLoopPool;
class NetAddr;
class TcpConnection;

class TcpServer : utils::noncopyable {
 public:
  using ConnPtr = std::shared_ptr<TcpConnection>;
  using ConnCallback = std::function<void(const ConnPtr &)>;
  using MsgCallback = std::function<void(const ConnPtr &, Buffer *)>;
  using CloseCallback = ConnCallback;

  TcpServer(EventLoop *loop, const NetAddr &addr, std::string name);
  ~TcpServer();

  void start();
  void setThreadNum(unsigned n);

  [[nodiscard]] std::string addr() const;

  void setConnCallback(ConnCallback);
  void setMsgCallback(MsgCallback);
  void setCloseCallback(CloseCallback);

 private:
  void newConnection(int fd, const NetAddr &addr);

  void removeConn(const ConnPtr &);

  using ConnMap = std::map<std::string, ConnPtr>;

  EventLoop *loop_;
  std::unique_ptr<NetAddr> addr_;
  std::unique_ptr<EventLoopPool> pool_;
  std::unique_ptr<Acceptor> acceptor_;
  ConnCallback connCallback_;
  MsgCallback msgCallback_;
  CloseCallback closeCallback_;
  ConnMap connections_;

  int connIdx_;
  std::string name_;
  std::atomic_bool started_;
};

}

#endif //LOTTA_TCP_SERVER_H
