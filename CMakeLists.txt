cmake_minimum_required(VERSION 3.4...3.18)
project(_simulation)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")  # or 11, 14, 17, 20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXXFLAGS "-O3 -Ofast -std=c++17")

include_directories(rq_analysis/include)



add_subdirectory(pybind11)
pybind11_add_module(_simulation SHARED rq_analysis/src/binding.cpp)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(_simulation
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})