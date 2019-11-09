//
// Created by suun on 11/7/19.
//

#ifndef LOTTA_UTILS_THIS_THREAD_H
#define LOTTA_UTILS_THIS_THREAD_H

#include <cstddef>

namespace lotta::utils::this_thread {

extern thread_local size_t t_cachedId;
size_t id();
}

#endif //LOTTA_UTILS_THIS_THREAD_H
