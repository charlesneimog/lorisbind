cmake_minimum_required(VERSION 3.20)

project(lorisbind)

add_subdirectory(Libraries/loris)
set_target_properties(loris PROPERTIES POSITION_INDEPENDENT_CODE ON)

execute_process(COMMAND python -m pybind11 --cmakedir
                OUTPUT_VARIABLE pybind11_CMAKE_DIR)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIR})
pybind11_add_module(_lorisbind MODULE Sources/lorisbind.cpp)

target_link_libraries(_lorisbind PRIVATE loris ${PYTHON_LIBRARIES})
include_directories(Libraries/loris/src)

install(TARGETS _lorisbind LIBRARY DESTINATION lorisbind)
