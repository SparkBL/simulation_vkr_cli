cmake_minimum_required(VERSION 3.4...3.18)
project(q_analysis)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")  # or 11, 14, 17, 20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3 -Ofast -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Ofast -std=c++17")
include_directories(q_analysis/include)



add_subdirectory(pybind11)
pybind11_add_module(simulation q_analysis/src/binding.cpp)
set_target_properties(simulation PROPERTIES OUTPUT_NAME q_analysis/simulation)


target_compile_definitions(simulation
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
