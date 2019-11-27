//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_SERVER_H
#define LOTTA_SERVER_H

#include "lotta/http/state.h"
#include <functional>
#include <memory>
#include <unordered_map>

namespace std {

template<>
struct hash<std::pair<lotta::http::Method, string>> {
  size_t operator()(const std::pair<lotta::http::Method, string> &key) const {
    return hash<int>()(static_cast<int>(key.first)) &
        hash<string>()(key.second);
  }
};

}

namespace lotta {

class Buffer;
class EventLoop;
class NetAddr;
class TcpServer;
class TcpConnection;

namespace http {

class Context;

class Server {
 public:
  using RouteFunc = std::function<void(Context *)>;

  Server(EventLoop *loop, const NetAddr &addr, std::string);
  ~Server();

  void run();
  void route(Method method, const std::string &path, RouteFunc f);

 private:
  using TcpConnPtr = std::shared_ptr<TcpConnection>;
  static void connCallback(const TcpConnPtr &);
  void msgCallback(const TcpConnPtr &, Buffer *);
  static void closeCallback(const TcpConnPtr &);

  std::unique_ptr<TcpServer> tcpServer_;
  std::unordered_map<std::pair<
      Method, std::string>, RouteFunc> routes_;
};

}

}

#endif //LOTTA_SERVER_H
