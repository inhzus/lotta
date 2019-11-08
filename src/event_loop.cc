//
// Created by suun on 11/7/19.
//

#include "lotta/event_loop.h"
#include "spdlog/spdlog.h"

using namespace lotta;

thread_local EventLoop *t_eventLoopThisThread = nullptr;

