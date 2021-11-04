cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
# set the project name
project(rqsim)
#list(APPEND CMAKE_PREFIX_PATH "easy_profiler/lib/cmake/easy_profiler")

set (INCLUDE_DIR include)
set(SOURCES
    main.cpp
    ${INCLUDE_DIR}/components.hpp
    ${INCLUDE_DIR}/config.hpp
    ${INCLUDE_DIR}/delay.hpp
    ${INCLUDE_DIR}/env.hpp
    ${INCLUDE_DIR}/json.hpp
    ${INCLUDE_DIR}/node.hpp
    ${INCLUDE_DIR}/orbit.hpp
    ${INCLUDE_DIR}/request.hpp
    ${INCLUDE_DIR}/stats.hpp
    ${INCLUDE_DIR}/stream.hpp
    ${INCLUDE_DIR}/router.hpp
   # ${INCLUDE_DIR}/easy/
)
include(GNUInstallDirs)



#find_package(easy_profiler REQUIRED)
# add the executable
add_executable(rqsim ${SOURCES})
target_include_directories(rqsim PRIVATE ${INCLUDE_DIR})

#target_link_libraries(rqsim easy_profiler) 