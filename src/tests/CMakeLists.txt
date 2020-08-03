cmake_minimum_required(VERSION 3.5)
project(Tests)

set(TESTS_BIN tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

#****************************************************************************************************
set(TESTS_SOURSES
    startup_tests.cpp
    test_request.cpp
    test_response.cpp
    test_datetime.cpp
)

#****************************************************************************************************
set_target_properties(${TESTS} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)
#****************************************************************************************************
add_executable(${TESTS_BIN}     ${TESTS_SOURSES})
target_include_directories(${TESTS_BIN} PUBLIC .)
include_directories(${GTEST_INCLUDE_DIR})
find_package(GTest REQUIRED)
find_library(UnitTests gtest)
#****************************************************************************************************

target_link_libraries (${TESTS_BIN} ${CMAKE_THREAD_LIBS_INIT} ${UnitTests} ${PROTOCOL_LIB_NAME})
