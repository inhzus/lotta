//
// Created by suun on 11/8/19.
//

#ifndef LOTTA_UTILS_SINGLETON_H
#define LOTTA_UTILS_SINGLETON_H

#include "lotta/utils/noncopyable.h"

namespace lotta::utils {

template<typename T>
class Singleton : noncopyable {
 public:
  static T &instance();
};
template<typename T>
T &Singleton<T>::instance() {
  static T t;
  return t;
}

}

#endif //LOTTA_UTILS_SINGLETON_H
