cmake_minimum_required(VERSION 3.18)

# set the project name, version, description, homepage and language
project(lispy 
    VERSION 0.0.3
    DESCRIPTION "Build Your Own Lisp project"
    HOMEPAGE_URL "http://buildyourownlisp.com"
    LANGUAGES C
)

configure_file(src/version.h.in version.h)

# specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

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
    mpc                             # for MPC parsing features
)

if(${CMAKE_HOST_APPLE})
target_link_libraries(lispy PRIVATE 
    edit                            # for readline and add_history
)
elseif(${CMAKE_HOST_UNIX})
target_link_libraries(lispy PRIVATE
    readline		    	    # for readline
    history			    # for add_history
)
endif()
