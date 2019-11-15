
//
// Created by suun on 11/12/19.
//

#include "lotta/net_addr.h"
#include "lotta/utils/logging.h"
#include <arpa/inet.h>

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

namespace lotta {

NetAddr::NetAddr() : addr_{} {}

NetAddr::NetAddr(uint16_t port, bool loopback, bool ipv6) : addr_{} {
  if (ipv6) {
    auto addr = reinterpret_cast<sockaddr_in6 *>(&addr_);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = loopback ? in6addr_loopback : in6addr_any;
  } else {
    auto addr = reinterpret_cast<sockaddr_in *>(&addr_);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(loopback ? INADDR_LOOPBACK : INADDR_ANY);
  }
}

NetAddr::NetAddr(const char *host, uint16_t port, bool ipv6) : addr_{} {
  int ret;
  if (ipv6) {
    auto addr = reinterpret_cast<sockaddr_in6 *>(&addr_);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    ret = inet_pton(AF_INET6, host, &addr->sin6_addr);
  } else {
    auto addr = reinterpret_cast<sockaddr_in *>(&addr_);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    ret = inet_pton(AF_INET, host, &addr->sin_addr);
  }
  if (ret == 0) {
    SPDLOG_ERROR("host does not contain a character string representing a "
                 "valid network address in the specified address family");
  } else if (ret == -1) {
    SPDLOG_ERROR("EAFNOSUPPORT");
  }
}

NetAddr::NetAddr(const sockaddr_in &addr) : addr_{} {
  memcpy(&addr_, &addr, sizeof(addr));
}

NetAddr::NetAddr(const sockaddr_in6 &addr6) : addr_{} {
  memcpy(&addr_, &addr6, sizeof(addr6));
}
sa_family_t NetAddr::family() const {
  return addr_.ss_family;
}
uint16_t NetAddr::port() const {
  if (addr_.ss_family == AF_INET6) {
    return ntohs(reinterpret_cast<const sockaddr_in6 *>(&addr_)->sin6_port);
  } else {
    return ntohs(reinterpret_cast<const sockaddr_in *>(&addr_)->sin_port);
  }
}
std::string NetAddr::host() const {
  char buf[64];
  if (addr_.ss_family == AF_INET6) {
    inet_ntop(
        AF_INET6, &reinterpret_cast<const sockaddr_in6 *>(&addr_)->sin6_addr,
        buf, INET6_ADDRSTRLEN);
  } else {
    inet_ntop(
        AF_INET, &reinterpret_cast<const sockaddr_in *>(&addr_)->sin_addr,
        buf, INET_ADDRSTRLEN);
  }
  return std::string(buf);
}
std::string NetAddr::addr() const {
  std::string s(host());
  s.push_back(':');
  s.append(std::to_string(port()));
  return s;
}

}
