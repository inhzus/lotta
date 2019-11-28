//
// Created by suun on 11/21/19.
//

#ifndef LOTTA_TCP_CONNECTION_H
#define LOTTA_TCP_CONNECTION_H

#include "lotta/buffer.h"
#include "lotta/net_addr.h"
#include "lotta/utils/noncopyable.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>

namespace lotta {

class Buffer;
class Channel;
class EventLoop;
class NetAddr;
class Socket;

class TcpConnection : utils::noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  using ConnPtr = std::shared_ptr<TcpConnection>;
  using ConnCallback = std::function<void(const ConnPtr &)>;
  using MsgCallback = std::function<void(const ConnPtr &, Buffer *)>;
  using CloseCallback = ConnCallback;
  using RmConnCallback = ConnCallback;

  TcpConnection(
      EventLoop *loop,
      std::string name,
      int fd,
      NetAddr localAddr,
      NetAddr peerAddr
  );
  virtual ~TcpConnection();

  void handleRead();
  void handleWrite();
  void handleClose();
  void handleError();

  void connEstablished();

  void send(const std::string &msg);
  void sendTask(const std::string &msg);

  void shutdown();
  void shutdownTask();

  void close();

  void setConnCallback(ConnCallback);
  void setMsgCallback(MsgCallback);
  void setCloseCallback(CloseCallback);
  void setRmConnCallback(RmConnCallback);

  [[nodiscard]] EventLoop *loop() { return loop_; }
  [[nodiscard]] std::string name() { return name_; }

 private:
// TODO friend class TcpServer;
// hide members after implementation of TcpServer & TcpConnection
  enum class State { Connecting, Connected, Disconnecting, Disconnected };

  EventLoop *loop_;
  std::string name_;
  std::atomic<State> state_;

  std::unique_ptr<Channel> channel_;
  std::unique_ptr<Socket> socket_;
  NetAddr localAddr_;
  NetAddr peerAddr_;
  ConnCallback connCallback_;
  MsgCallback msgCallback_;
  CloseCallback closeCallback_;
  RmConnCallback rmConnCallback_;

  Buffer inBuf;
  Buffer outBuf_;
};

}

#endif //LOTTA_TCP_CONNECTION_H
