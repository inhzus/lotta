if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

add_library(net_src
        acceptor.cc
        buffer.cc
        channel.cc
        event_loop.cc
        net_addr.cc
        poller.cc
        poll_poller.cc
        socket.cc
        tcp_connection.cc
        tcp_server.cc
        timer.cc
        timer_queue.cc
        utils/logging.cc
        utils/this_thread.cc
        )
target_link_libraries(net_src
        PRIVATE spdlog::spdlog)
