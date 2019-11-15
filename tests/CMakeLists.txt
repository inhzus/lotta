add_executable(net_test
    main.cc
    acceptor.cc
    capture.cc
    event_loop.cc
    timer_queue.cc)
enable_testing()
find_package(GTest REQUIRED)
target_link_libraries(net_test GTest::GTest GTest::Main net_src)
