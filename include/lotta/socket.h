//
// Created by suun on 11/10/19.
//

#ifndef LOTTA_SOCKET_H
#define LOTTA_SOCKET_H

#include <unistd.h>

namespace lotta {
namespace socket {
inline ssize_t read(int fd, void *buf, size_t n) {
  return ::read(fd, buf, n);
}

inline ssize_t write(int fd, const void *buf, size_t n) {
  return ::write(fd, buf, n);
}

inline int close(int fd) {
  return ::close(fd);
}
}

class NetAddr;

class Socket {
 public:
  explicit Socket(int fd);
  ~Socket();

  Socket &listen();
  Socket &bind(const NetAddr &addr);
  int accept(NetAddr &addr);

  Socket &setReuseAddr(bool);
  Socket &setReusePort(bool);
  Socket &setKeepAlive(bool);
  Socket &setTcpNoDelay(bool);

  [[nodiscard]] int fd() const;

 private:
  int fd_;
};

}

#endif //LOTTA_SOCKET_H
