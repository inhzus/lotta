//
// Created by suun on 11/7/19.
//

#include "lotta/utils/this_thread.h"
#include <unistd.h>
#include <sys/syscall.h>

namespace lotta::utils::this_thread {

thread_local size_t t_cachedId = 0;

size_t id() {
  if (t_cachedId == 0) {
    t_cachedId = syscall(SYS_gettid);
//    t_cachedId = std::hash<std::thread::id>{}(std::this_thread::get_id());
  }
  return t_cachedId;
}

}
