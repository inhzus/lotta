if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

add_library(lotta_lib
        acceptor.cc
        buffer.cc
        channel.cc
        connector.cc
        event_loop.cc
        event_loop_pool.cc
        event_loop_thread.cc
        net_addr.cc
        poller.cc
        poll_poller.cc
        socket.cc
        tcp_client.cc
        tcp_connection.cc
        tcp_server.cc
        timer.cc
        timer_queue.cc
        http/request.cc
        http/response.cc
        http/state.cc
        http/server.cc
        utils/logging.cc
        utils/this_thread.cc)
target_link_libraries(lotta_lib
        PRIVATE spdlog::spdlog)
