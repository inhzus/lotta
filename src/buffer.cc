//
// Created by suun on 11/18/19.
//

#include "lotta/buffer.h"
#include "lotta/socket.h"
#include "lotta/utils/logging.h"
#include <sys/ioctl.h>

namespace lotta {

const size_t Buffer::kInitLen = 2u << 10u;

ssize_t Buffer::readFrom(int fd) {
  int n;
  ioctl(fd, FIONREAD, &n);
  std::vector<char> buf;
  buf.reserve(n);
  ssize_t ret = socket::read(fd, &*buf.begin(), n);
  assert(ret == n);
  append(&*buf.begin(), ret);
  if (n < 0 || ret < 0) {
    SPDLOG_ERRNO_F("buffer read from fd error: unread{}, read{}", n, ret);
  }
  return ret;
}

}
