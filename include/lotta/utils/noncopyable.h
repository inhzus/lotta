//
// Created by suun on 11/7/19.
//

#ifndef LIBNET_NONCOPYABLE_H
#define LIBNET_NONCOPYABLE_H

namespace lotta {

class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}

#endif //LIBNET_NONCOPYABLE_H
