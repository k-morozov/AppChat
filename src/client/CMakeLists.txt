cmake_minimum_required(VERSION 3.5)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

set(Connection_dir          connection)
set(Channel_dir             channel)

#****************************************************************************************************
set(CHANNELS_SRC
    ${Channel_dir}/iroom.h ${Channel_dir}/iroom.cpp
    ${Channel_dir}/channels_manager.h ${Channel_dir}/channels_manager.cpp
    ${Channel_dir}/channel.h ${Channel_dir}/channel.cpp
)
set(CONNECTIONS_SRC
    ${Connection_dir}/isubscriber.h ${Connection_dir}/isubscriber.cpp
    ${Connection_dir}/connection.h ${Connection_dir}/connection.cpp
)

#****************************************************************************************************
set(SERVER_SOURCES
    startup_server.cpp
    server.h
    ${CONNECTIONS_SRC}
    ${CHANNELS_SRC}
)
add_executable(${CLIENT_BIN}  ${SERVER_SOURCES})
target_include_directories(${CLIENT_BIN} PUBLIC .)
#****************************************************************************************************
find_package(Boost COMPONENTS system date_time program_options regex REQUIRED)
target_link_libraries (${CLIENT_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${STORAGE_LIB_NAME} ${PROTOCOL_LIB_NAME} ${LOGGER_LIB_NAME})
