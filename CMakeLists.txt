cmake_minimum_required(VERSION 3.14)
project(lotta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

include_directories(include)
add_subdirectory(external)
include_directories(external/spdlog/include)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)
