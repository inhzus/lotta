//
// Created by suun on 11/9/19.
//

#include "lotta/utils/logging.h"

namespace lotta {

std::shared_ptr<spdlog::logger> logger;

class LoggerInitializer {
 public:
  LoggerInitializer() {
    logger = spdlog::stdout_color_mt("lotta");
    logger->set_level(spdlog::level::trace);
  }
};

static LoggerInitializer initializer; // NOLINT(cert-err58-cpp)
}
