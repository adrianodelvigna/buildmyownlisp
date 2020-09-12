cmake_minimum_required(VERSION 3.18)

project(lispy VERSION 0.0.1)

configure_file(src/version.h.in version.h)

add_executable(lispy src/lispy.c)
target_include_directories(lispy PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(lispy PRIVATE edit)