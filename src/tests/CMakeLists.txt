cmake_minimum_required(VERSION 3.5)
set(TESTS_BIN tests)

set(TESTS_SOURSES
    startup_tests.cpp
    test_request.cpp
    test_response.cpp
    test_datetime.cpp
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set_target_properties(${TESTS} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)
#****************************************************************************************************
add_executable(${TESTS_BIN}
    ${TESTS_SOURSES}
)
include_directories(${GTEST_INCLUDE_DIR})

find_package(GTest REQUIRED)
#find_library(UnitTests gtest)
#****************************************************************************************************

target_link_libraries (${TESTS_BIN} ${CMAKE_THREAD_LIBS_INIT} gtest ${PROTOCOL_LIB_NAME})

add_test(NAME ${TESTS_BIN} COMMAND ${TESTS_BIN})
