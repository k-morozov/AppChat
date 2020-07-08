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

set(SRC     src/)
set(LIB     lib/)

set(Client  ${SRC}client/)
set(Tests   ${SRC}tests/)

set(Server_lib_dir      ${LIB}server/)
set(Protocol_lib_dir    ${LIB}protocol)
set(Logger_lib_dir      ${LIB}log)
set(Storage_lib_dir     ${LIB}/storage)

set(SERVER_LIB_NAME     server_lib)
set(PROTOCOL_LIB_NAME   protocol_lib)
set(LOGGER_LIB_NAME     logger_lib)
set(STORAGE_LIB_NAME    sqlite_db)
#****************************************************************************************************
set(SERVER_SOURCES
    ${SRC}server/startup_server.cpp
)

set(CLIENT_SOURCES
    ${Client}/startup_client.cpp
    ${Client}/client.h  ${Client}/client.cpp
    ${SRC}/gui/mainwindow.h ${SRC}/gui/mainwindow.cpp
    ${SRC}/gui/mainwindow.ui
    ${Client}/control.h ${Client}/control.cpp
)

set(TESTS_SOURSES
    ${Tests}/startup_tests.cpp
    ${Tests}/test_request.cpp
    ${Tests}/test_response.cpp
)
#****************************************************************************************************
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#****************************************************************************************************
# add projects
add_executable(${SERVER_BIN}    ${SERVER_SOURCES})
add_executable(${CLIENT_BIN}    ${CLIENT_SOURCES})
add_executable(${TESTS_BIN}     ${TESTS_SOURSES})

target_include_directories(${SERVER_BIN}    PUBLIC ${SRC})
target_include_directories(${CLIENT_BIN}    PUBLIC ${SRC})
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
#find_package(SQLite3 REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

find_library(UnitTests gtest)
#****************************************************************************************************
add_subdirectory(${Server_lib_dir})
add_subdirectory(${Protocol_lib_dir})
add_subdirectory(${Logger_lib_dir})
add_subdirectory(${Storage_lib_dir})

target_link_libraries (${SERVER_BIN} ${SERVER_LIB_NAME})
target_link_libraries (${CLIENT_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} Qt5::Widgets ${PROTOCOL_LIB_NAME})
target_link_libraries (${TESTS_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${UnitTests} ${PROTOCOL_LIB_NAME})
#****************************************************************************************************
if(Boost_FOUND)
    include_directories(${BOOST_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_definitions(${Boost_DEFINITIONS})

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${CLIENT_BIN} ${Boost_LIBRARIES})
    target_link_libraries(${CLIENT_BIN} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_SYSTEM_LIBRARY})
endif()
