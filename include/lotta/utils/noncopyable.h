//
// Created by suun on 11/7/19.
//

#ifndef LOTTA_UTILS_NONCOPYABLE_H
#define LOTTA_UTILS_NONCOPYABLE_H

namespace lotta::utils {

class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}

#endif //LOTTA_UTILS_NONCOPYABLE_H
