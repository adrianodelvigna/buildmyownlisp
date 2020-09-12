cmake_minimum_required(VERSION 3.18)

project(lispy VERSION 0.0.2)

configure_file(src/version.h.in version.h)

# Creates a static lib out of mpc code
add_library(mpc vendor/mpc/mpc.c)

# Project executable
add_executable(lispy 
    src/lispy.c
)
target_include_directories(lispy PUBLIC 
    "${PROJECT_BINARY_DIR}"         # for version.h
    "${PROJECT_SOURCE_DIR}/vendor"  # for thid-party dependencies
)
target_link_libraries(lispy PRIVATE 
    edit                            # for readline and add_history
    mpc                             # for MPC parsing features
)