cmake_minimum_required(VERSION 2.8)

set(EX "safe")

set(SOURCE "BST_thread_safe.c")

set(CMAKE_C_FLAGS "-pthread -g")

add_executable(${EX} ${SOURCE})

include_directories(lib)

add_subdirectory(lib)

target_link_libraries(${EX} lib)
