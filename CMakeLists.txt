cmake_minimum_required(VERSION 3.5)
project(AppChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

#****************************************************************************************************
set(SRC     src)
set(LIB     lib)
set(Tests   ${SRC}/tests/)
set(TESTS_BIN   tests)

set(PROTOCOL_LIB_NAME        protocol_lib)
set(LOGGER_LIB_NAME          logger_lib)
set(STORAGE_LIB_NAME         sqlite_db)
set(BACKTRACE_LIB_NAME       backtrace)
set(TRACEDEXCEPTION_LIB_NAME tracedexception_lib)
#****************************************************************************************************
set(TESTS_SOURSES
    ${Tests}/startup_tests.cpp
    ${Tests}/test_request.cpp
    ${Tests}/test_response.cpp
    ${Tests}/test_datetime.cpp
)
#****************************************************************************************************
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#****************************************************************************************************
add_executable(${TESTS_BIN}     ${TESTS_SOURSES})

add_subdirectory(${SRC}/server)
add_subdirectory(${SRC}/client)
add_subdirectory(${LIB}/protocol)
add_subdirectory(${LIB}/log)
add_subdirectory(${LIB}/storage)
add_subdirectory(${LIB}/libbacktrace)
add_subdirectory(${LIB}/traced_exception)

target_include_directories(${TESTS_BIN}     PUBLIC ${SRC})
#****************************************************************************************************
set_target_properties(${TESTS} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

#****************************************************************************************************
enable_testing()
include_directories(${GTEST_INCLUDE_DIR})

#****************************************************************************************************
find_package(Boost COMPONENTS system date_time filesystem program_options regex thread graph REQUIRED)
find_package (Threads)
find_package(GTest REQUIRED)
find_library(UnitTests gtest)
#****************************************************************************************************

target_link_libraries (${TESTS_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${UnitTests} ${PROTOCOL_LIB_NAME})
#****************************************************************************************************

##### Documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYFILE ${CMAKE_SOURCE_DIR}/Doxyfile)
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen")
    unset(DOXYFILE)
endif()
#####
