cmake_minimum_required(VERSION 3.5)
project(TracedExceptionExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_executable(traced_exception_example traced_exception.h example.cpp)

find_package(Boost REQUIRED)

target_link_libraries(traced_exception_example ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
