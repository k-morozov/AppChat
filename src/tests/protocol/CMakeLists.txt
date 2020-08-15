cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TESTS_PROTOCOL tests_protocol)
set(TESTS_PROTOCOL_SOURSES
    startup_tests.cpp
    test_request.cpp
    test_response.cpp
    test_datetime.cpp
)

add_executable(${TESTS_PROTOCOL}
    ${TESTS_PROTOCOL_SOURSES}
)
find_package(Boost REQUIRED COMPONENTS system date_time unit_test_framework REQUIRED)
target_link_libraries (${TESTS_PROTOCOL}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    protocol_lib
)

target_include_directories(${TESTS_PROTOCOL} PRIVATE
    ${Boost_INCLUDE_DIR}
)

add_test(NAME ${TESTS_PROTOCOL} COMMAND ${TESTS_PROTOCOL})
