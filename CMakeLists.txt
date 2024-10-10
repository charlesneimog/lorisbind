cmake_minimum_required(VERSION 3.20)

project(lorisbind)

add_subdirectory(Libraries/loris)
set_target_properties(loris PROPERTIES POSITION_INDEPENDENT_CODE ON)

find_package(PythonExtensions REQUIRED)
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIR})
pybind11_add_module(_lorisbind MODULE Sources/lorisbind.cpp)

target_link_libraries(_lorisbind PRIVATE loris ${PYTHON_LIBRARIES})
include_directories(Libraries/loris/src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")

install(TARGETS _lorisbind LIBRARY DESTINATION lorisbind)
