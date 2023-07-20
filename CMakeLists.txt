
# set min version
cmake_minimum_required(VERSION 3.17)

project(NumpyAppender)

# cpp version
set(CMAKE_CXX_STANDARD 20)

# add new shared library
add_library(NumpyAppender SHARED src/numpy_appender.hpp)
set_target_properties(NumpyAppender PROPERTIES LINKER_LANGUAGE CXX)

add_executable(NumpyAppenderTest main.cpp)
target_link_libraries(NumpyAppenderTest NumpyAppender)
include_directories(NumpyAppenderTest ${CMAKE_CURRENT_SOURCE_DIR}/src)
