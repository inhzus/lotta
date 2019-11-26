add_executable(net_test
        main.cc
        acceptor.cc
        buffer.cc
        capture.cc
        connector.cc
        event_loop.cc
        event_loop_thread.cc
        tcp_server.cc
        timer_queue.cc)
enable_testing()
find_package(GTest REQUIRED)
target_link_libraries(net_test GTest::GTest GTest::Main net_src)
