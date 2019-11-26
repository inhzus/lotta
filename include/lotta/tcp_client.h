//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_TCP_CLIENT_H
#define LOTTA_TCP_CLIENT_H

#include "lotta/utils/logging.h"

namespace lotta {

class Buffer;
class Connector;
class EventLoop;
class NetAddr;
class TcpConnection;

class TcpClient {
 public:
  using ConnPtr = std::shared_ptr<TcpConnection>;
  using ConnCallback = std::function<void(const ConnPtr &)>;
  using MsgCallback = std::function<void(const ConnPtr &, Buffer *)>;
  using CloseCallback = ConnCallback;

  TcpClient(EventLoop *loop, const NetAddr &addr, std::string name);
  ~TcpClient();

  void connect();

  [[nodiscard]] ConnPtr conn();

 private:
  void newConnection(int fd);
  void removeConn(const TcpClient::ConnPtr &conn);

  void setConnCallback(ConnCallback);
  void setMsgCallback(MsgCallback);
  void setCloseCallback(CloseCallback);

  EventLoop *loop_;
  std::unique_ptr<Connector> connector_;
  ConnCallback connCallback_;
  MsgCallback msgCallback_;
  CloseCallback closeCallback_;

  std::mutex mtx_;
  ConnPtr connection_;  // guarded by mtx_
  unsigned connIdx_;
  std::string name_;

};

}

#endif //LOTTA_TCP_CLIENT_H
