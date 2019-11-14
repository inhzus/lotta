//
// Created by suun on 11/12/19.
//

#ifndef LOTTA_NET_ADDR_H
#define LOTTA_NET_ADDR_H

#include "netinet/in.h"
#include <string>

namespace lotta {

struct NetAddr {
 public:
  explicit NetAddr(uint16_t port, bool loopback = false, bool ipv6 = false);
  NetAddr(const char *host, uint16_t port, bool ipv6 = false);

  explicit NetAddr(const sockaddr_in &);
  explicit NetAddr(const sockaddr_in6 &);

  [[nodiscard]] sa_family_t family() const;
  [[nodiscard]] uint16_t port() const;
  [[nodiscard]] std::string host() const;
  [[nodiscard]] std::string addr() const;
  sockaddr_storage addr_;
};

}

#endif //LOTTA_NET_ADDR_H
