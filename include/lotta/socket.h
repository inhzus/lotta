//
// Created by suun on 11/10/19.
//

#ifndef LOTTA_SOCKET_H
#define LOTTA_SOCKET_H

#include <unistd.h>

namespace lotta::socket {

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

#endif //LOTTA_SOCKET_H
