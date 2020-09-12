cmake_minimum_required(VERSION 3.18)

project(lispy VERSION 0.0.2)

configure_file(src/version.h.in version.h)

add_library(mpc vendor/mpc/mpc.c)

add_executable(lispy src/lispy.c)
target_include_directories(lispy PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/vendor"
)
target_link_libraries(lispy PRIVATE 
    edit 
    mpc
)