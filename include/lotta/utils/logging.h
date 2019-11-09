//
// Created by suun on 11/9/19.
//

#ifndef LOTTA_UTILS_LOGGING_H
#define LOTTA_UTILS_LOGGING_H

#include "lotta/utils/singleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace lotta {

spdlog::logger *logger();

}

#endif //LOTTA_UTILS_LOGGING_H
