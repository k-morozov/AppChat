cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TESTS_PROTOCOL tests_protocol)
set(TESTS_PROTOCOL_SOURSES
    startup_tests.cpp
    test_request.cpp
)

add_executable(${TESTS_PROTOCOL}
    ${TESTS_PROTOCOL_SOURSES}
)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS system date_time unit_test_framework REQUIRED)
target_link_libraries (${TESTS_PROTOCOL}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    protocol_lib
    project_sanitizers
    pthread
    messages_pb_lib
)

target_include_directories(${TESTS_PROTOCOL} PRIVATE
    ${Boost_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME ${TESTS_PROTOCOL} COMMAND ${TESTS_PROTOCOL})
