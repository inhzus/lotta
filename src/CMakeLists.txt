if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

add_library(net_src
        channel.cc
        event_loop.cc
        poller.cc
        poll_poller.cc
        timer.cc
        timer_queue.cc
        utils/logging.cc
        utils/this_thread.cc
        )
target_link_libraries(net_src
        PRIVATE spdlog::spdlog)
