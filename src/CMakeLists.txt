if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

add_library(net_src
        event_loop.cc
        utils/current_thread.cc)
target_link_libraries(net_src
        PRIVATE spdlog::spdlog)
