add_executable(lotta_test
        main.cc
        acceptor.cc
        buffer.cc
        capture.cc
        connector.cc
        event_loop.cc
        event_loop_thread.cc
        http_server.cc
        tcp_client.cc
        tcp_server.cc
        timer_queue.cc)
enable_testing()
find_package(GTest REQUIRED)
target_link_libraries(lotta_test GTest::GTest GTest::Main lotta_lib)
