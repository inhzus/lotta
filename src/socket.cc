//
// Created by suun on 11/12/19.
//

#include "lotta/socket.h"
#include "lotta/net_addr.h"
#include "lotta/utils/logging.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>

#define HANDLE_ERR(condition) do { \
  if (condition) { \
    SPDLOG_CRITICAL("fail({}) to {}", std::strerror(errno), __FUNCTION__); \
  } \
}while(0)

namespace lotta {

Socket::Socket(int fd) : fd_(fd) {}
Socket::~Socket() {
  socket::close(fd_);
}
void Socket::listen() {
  int n = ::listen(fd_, SOMAXCONN);
  HANDLE_ERR(n < 0);
}
void Socket::bind(const NetAddr &addr) {
  int n = ::bind(fd_, reinterpret_cast<const sockaddr *>(&addr.addr_),
                 sizeof(sockaddr_in6));
  HANDLE_ERR(n < 0);
}
int Socket::accept(NetAddr &addr) {
  socklen_t n = sizeof(sockaddr_in6);
  int fd = ::accept4(
      fd_, reinterpret_cast<sockaddr *>(&addr.addr_), &n,
      SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (fd < 0) {
    switch (int err = errno) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO:
      case EPERM:
      case EMFILE:SPDLOG_WARN("accept encounter error {}", err);
        break;
      default:SPDLOG_CRITICAL("fail({}) to accept", err);
        break;
    }
  }
  return fd;
}

void Socket::setReuseAddr(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
  HANDLE_ERR(n < 0);
}
void Socket::setReusePort(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &v, sizeof(v));
  HANDLE_ERR(n < 0);
}
void Socket::setKeepAlive(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &v, sizeof(v));
  HANDLE_ERR(n < 0);
}
void Socket::setTcpNoDelay(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
  HANDLE_ERR(n < 0);
}
int Socket::fd() const {
  return fd_;
}

}
