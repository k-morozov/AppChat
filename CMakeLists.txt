cmake_minimum_required(VERSION 3.5)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    #message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} doesn't have C++17 support.")
endif()
message(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT Chat)
set(SERVER server)
set(CLIENT client)
set(TESTS tests)

set(SRC     src/)
set(Server  ${SRC}server/)
set(Client  ${SRC}client/)
set(Tests   ${SRC}tests/)

set(Connection ${Server}connection)
set(Channel ${Server}channel)
set(Protocol ${SRC}protocol)
set(Storage ${SRC}storage)
set(Logger ${SRC}log)

set(PROTOCOL
    ${Protocol}/protocol.h ${Protocol}/protocol.cpp
    ${Protocol}/command_table.h
    ${Protocol}/request/request.h
    ${Protocol}/response/response.h
    ${Protocol}/request/input_request.h
    ${Protocol}/request/input_request.cpp
    ${Protocol}/request/transport_request.h
    ${Protocol}/request/transport_request.cpp
    ${Protocol}/request/control_request.h

    ${Protocol}/response/input_response.h
    ${Protocol}/response/input_response.cpp
    ${Protocol}/response/transport_response.h

    )
set(CHANNELS
    ${Channel}/iroom.h ${Channel}/iroom.cpp
    ${Channel}/channels_manager.h ${Channel}/channels_manager.cpp
    ${Channel}/channel.h ${Channel}/channel.cpp
    )
set(CONNECTION
    ${Connection}/isubscriber.h ${Connection}/isubscriber.cpp
    ${Connection}/connection.h ${Connection}/connection.cpp
)

set(STORAGE
     ${Storage}/database.h ${Storage}/database.cpp
)
set(LOGEGR
    ${Logger}/logger.h ${Logger}/logger.cpp
)

#****************************************************************************************************
set(SERVER_SOURCES
    ${Server}/startup_server.cpp
    ${Server}/server.h

    ${CONNECTION}
    ${CHANNELS}
    ${PROTOCOL}
    ${STORAGE}
    ${LOGEGR}
)

set(CLIENT_SOURCES
    ${Client}/startup_client.cpp
    ${Client}/client.h  ${Client}/client.cpp
    ${SRC}/gui/mainwindow.h ${SRC}/gui/mainwindow.cpp
    ${SRC}/gui/mainwindow.ui
    ${Client}/control.h ${Client}/control.cpp
    ${PROTOCOL}
)

set(TESTS_SOURSES
    ${Tests}/startup_tests.cpp
    ${Tests}/test_request.cpp
    ${Tests}/test_response.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# add projects
add_executable(${SERVER}    ${SERVER_SOURCES})
add_executable(${CLIENT}    ${CLIENT_SOURCES})
add_executable(${TESTS}     ${TESTS_SOURSES})

target_include_directories(${SERVER} PUBLIC ${SRC})
target_include_directories(${CLIENT} PUBLIC ${SRC})
target_include_directories(${TESTS} PUBLIC ${SRC})

set_target_properties(${SERVER} #${CLIENT}
    ${TESTS} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++2a;-O2;-Wall;-Wextra;-Wpedantic"
)
#****************************************************************************************************

enable_testing()
include_directories(${GTEST_INCLUDE_DIR})


find_package(Boost COMPONENTS system date_time filesystem program_options regex thread graph REQUIRED)
find_package (Threads)
find_package(GTest REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

find_library(UnitTests gtest)
find_library(LOGGER log4cplus)

target_link_libraries (${SERVER} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LOGGER} ${SQLite3_LIBRARIES})
target_link_libraries (${CLIENT} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LOGGER} Qt5::Widgets)
target_link_libraries (${TESTS} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${UnitTests})

if(Boost_FOUND)
    include_directories(${BOOST_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_definitions(${Boost_DEFINITIONS})

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${SERVER} ${Boost_LIBRARIES})
    target_link_libraries(${CLIENT} ${Boost_LIBRARIES})
    target_link_libraries(${SERVER} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    target_link_libraries(${CLIENT} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_SYSTEM_LIBRARY})
endif()
