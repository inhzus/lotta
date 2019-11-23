//
// Created by suun on 11/10/19.
//

#ifndef LOTTA_SOCKET_H
#define LOTTA_SOCKET_H

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
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

inline int socket(int domain) {
  return ::socket(
      domain,
      SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, // NOLINT
      IPPROTO_TCP);
}

inline int open(const char *file, int flags) {
  return ::open(file, flags);
}

sockaddr_storage getSockName(int fd);
int getSocketErrno(int fd);

}

class NetAddr;

class Socket {
 public:
  explicit Socket(int fd);
  ~Socket();

  void listen();
  void bind(const NetAddr &addr);
  int accept(NetAddr &addr);

  void setReuseAddr(bool);
  void setReusePort(bool);
  void setKeepAlive(bool);
  void setTcpNoDelay(bool);

  [[nodiscard]] int fd() const;

 private:
  int fd_;
};

}

#endif //LOTTA_SOCKET_H
