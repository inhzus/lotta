add_executable(net_test main.cc event_loop.cc)
target_link_libraries(net_test gtest_main gtest gmock_main gmock net_src)
