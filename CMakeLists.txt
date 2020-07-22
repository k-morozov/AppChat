cmake_minimum_required(VERSION 3.5)
project(AppChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#****************************************************************************************************
set(SERVER_BIN  server)
set(CLIENT_BIN  client)
set(TESTS_BIN   tests)

set(SRC     src)
set(LIB     lib/)

set(Tests   ${SRC}/tests/)

set(Server_src_dir      ${SRC}/server)
set(Client_src_dir      ${SRC}/client)
set(Protocol_lib_dir    ${LIB}protocol)
set(Logger_lib_dir      ${LIB}log)
set(Storage_lib_dir     ${LIB}/storage)

set(PROTOCOL_LIB_NAME   protocol_lib)
set(LOGGER_LIB_NAME     logger_lib)
set(STORAGE_LIB_NAME    sqlite_db)
#****************************************************************************************************
set(TESTS_SOURSES
    ${Tests}/startup_tests.cpp
    ${Tests}/test_request.cpp
    ${Tests}/test_response.cpp
    ${Tests}/test_datetime.cpp
)
#****************************************************************************************************
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#****************************************************************************************************
add_executable(${TESTS_BIN}     ${TESTS_SOURSES})

add_subdirectory(${Server_src_dir})
add_subdirectory(${Client_src_dir})
add_subdirectory(${Protocol_lib_dir})
add_subdirectory(${Logger_lib_dir})
add_subdirectory(${Storage_lib_dir})

target_include_directories(${SERVER_BIN}    PUBLIC ${Server_src_dir})
target_include_directories(${CLIENT_BIN}    PUBLIC ${Client_src_dir})
target_include_directories(${TESTS_BIN}     PUBLIC ${SRC})
#****************************************************************************************************
set_target_properties(${SERVER} #${CLIENT}
    ${TESTS} PROPERTIES
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
####
