//
// Created by suun on 11/9/19.
//

#ifndef LOTTA_UTILS_LOGGING_H
#define LOTTA_UTILS_LOGGING_H

#define SPDLOG_ACTIVE_LEVEL 0
#include "spdlog/spdlog.h"
#include <cassert>
#include <cstring>

#define SPDLOG_ERRNO() \
  do { \
    SPDLOG_ERROR("fail({}) to {}", std::strerror(errno), __FUNCTION__); \
  } while (0)

#define SPDLOG_ERRNO_F(fmt, ...) \
  do { \
    SPDLOG_ERROR("fail({}) to {}, " fmt, \
        std::strerror(errno), __FUNCTION__, ## __VA_ARGS__); \
  } while (0)

#define SPDLOG_ERRNO_L(LEVEL, fmt, ...) \
  do { \
    SPDLOG_##LEVEL("fail({}) to {}, " fmt, \
        std::strerror(errno), __FUNCTION__, ## __VA_ARGS__); \
  } while (0)

#define SPDLOG_ERRNO_IF(condition) \
  do { \
    if (condition) SPDLOG_ERRNO(); \
  } while (0)

#define SPDLOG_ERRNO_IF_F(condition, fmt, ...) \
  do { \
    if (condition) SPDLOG_ERROR("fail({}) to {}, " fmt, \
        std::strerror(errno), __FUNCTION__, ## __VA_ARGS__); \
  } while (0)

namespace lotta::utils::logging {

}

#endif //LOTTA_UTILS_LOGGING_H
