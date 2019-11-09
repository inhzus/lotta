//
// Created by suun on 11/9/19.
//

#include "lotta/utils/logging.h"

namespace lotta {

spdlog::logger *logger() {
  static std::shared_ptr<spdlog::logger> staticLogger = []() {
    auto innerLogger = spdlog::stdout_color_mt("lotta");
    innerLogger->set_level(spdlog::level::trace);
    innerLogger->info("logger initializing");
    return innerLogger;
  }();
  return staticLogger.get();
}

}
