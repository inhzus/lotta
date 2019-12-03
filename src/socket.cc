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

namespace lotta {
namespace socket {

int shutdownWrite(int fd) {
  int n = ::shutdown(fd, SHUT_WR);
  SPDLOG_ERRNO_IF_F(n < 0, "fd: {}", fd);
  return n;
}

sockaddr_storage getSockName(int fd) {
  sockaddr_storage addr{};
  auto len = static_cast<socklen_t >(sizeof(addr));
  int n = ::getsockname(fd, reinterpret_cast<sockaddr *>(&addr), &len);
  SPDLOG_ERRNO_IF_F(n < 0, "fd: {}", fd);
  return addr;
}

sockaddr_storage getPeerName(int fd) {
  sockaddr_storage addr{};
  auto len = static_cast<socklen_t>(sizeof(addr));
  int n = ::getpeername(fd, reinterpret_cast<sockaddr *>(&addr), &len);
  SPDLOG_ERRNO_IF_F(n < 0, "fd: {}", fd);
  return addr;
}

int getSocketErrno(int fd) {
  int val{};
  auto len = static_cast<socklen_t >(sizeof val);
  if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &val, &len) < 0) {
    return errno;
  }
  return val;
}

}

Socket::Socket(int fd) : fd_(fd) {}
Socket::~Socket() {
  socket::close(fd_);
}
void Socket::listen() {
  int n = ::listen(fd_, SOMAXCONN);
  SPDLOG_ERRNO_IF(n < 0);
}
void Socket::bind(const NetAddr &addr) {
  int n = ::bind(fd_, reinterpret_cast<const sockaddr *>(&addr.addr_),
                 sizeof(sockaddr_in6));
  SPDLOG_ERRNO_IF(n < 0);
}
int Socket::accept(NetAddr &addr) {
  socklen_t n = sizeof(sockaddr_in6);
  int fd = ::accept4(
      fd_, reinterpret_cast<sockaddr *>(&addr.addr_), &n,
      SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (fd < 0) {
    switch (errno) {
      case EAGAIN:
      case ENETDOWN:
      case EPROTO:
      case ENOPROTOOPT:
      case EHOSTDOWN:
      case ENONET:
      case EHOSTUNREACH:
      case EOPNOTSUPP:
      case ENETUNREACH:SPDLOG_ERRNO_L(WARN, "acceptable err");
        break;
      default:SPDLOG_ERRNO();
        break;
    }
  }
  return fd;
}

void Socket::setReuseAddr(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
  SPDLOG_ERRNO_IF(n < 0);
}
void Socket::setReusePort(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &v, sizeof(v));
  SPDLOG_ERRNO_IF(n < 0);
}
void Socket::setKeepAlive(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &v, sizeof(v));
  SPDLOG_ERRNO_IF(n < 0);
}
void Socket::setTcpNoDelay(bool b) {
  int v = b ? 1 : 0;
  int n = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
  SPDLOG_ERRNO_IF(n < 0);
}
int Socket::fd() const {
  return fd_;
}

}
