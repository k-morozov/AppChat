cmake_minimum_required(VERSION 3.5)
project(TracedException)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

set(TRACEDEXCEPTION_SRC
    traced_exception.h
    traced_exception.cpp
)

add_library(${TRACEDEXCEPTION_LIB_NAME} ${TRACEDEXCEPTION_SRC})
target_include_directories(${TRACEDEXCEPTION_LIB_NAME} PUBLIC .)

find_package(Boost REQUIRED)

target_link_libraries(${TRACEDEXCEPTION_LIB_NAME} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
