cmake_minimum_required(VERSION 3.5)

set(SERVER_NAME appchat-server)
add_executable(${SERVER_NAME})

set(DIR_CONNECTION  connection)
set(DIR_CHANNEL     channel)
set(DIR_LOG         log)
set(DIR_STORAGE     storage)

set(CMAKE_CXX_FLAGS "-lboost_program_options")

set(CHANNELS_SRC
    ${DIR_CHANNEL}/iroom.h
    ${DIR_CHANNEL}/iroom.cpp
    ${DIR_CHANNEL}/channels_manager.h
    ${DIR_CHANNEL}/channels_manager.cpp
    ${DIR_CHANNEL}/channel.h
    ${DIR_CHANNEL}/channel.cpp
)
set(CONNECTIONS_SRC
    ${DIR_CONNECTION}/isubscriber.h
    ${DIR_CONNECTION}/isubscriber.cpp
    ${DIR_CONNECTION}/connection.h
    ${DIR_CONNECTION}/connection.cpp
    ${DIR_CONNECTION}/connection_manager.h
    ${DIR_CONNECTION}/connection_manager.cpp
)
set(LOGGER_SRC
    ${DIR_LOG}/logger.h
    ${DIR_LOG}/logger.cpp
)

find_library(SQLITE3_LIBRARY NAMES sqlite3)
set(STORAGE_SRC
     ${DIR_STORAGE}/database.h
     ${DIR_STORAGE}/database.cpp
)

set(SERVER_SOURCES
    startup_server.cpp
    server.h
    ${CONNECTIONS_SRC}
    ${CHANNELS_SRC}
    ${LOGGER_SRC}
    ${SQLITE3_LIBRARY}
    ${STORAGE_SRC}
)

target_include_directories(${SERVER_NAME}
    PRIVATE
        ${Boost_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

unset(PROTOCOL_INCLUDE_DIRECTORIES)

target_sources(${SERVER_NAME} PRIVATE ${SERVER_SOURCES})

set_target_properties(${SERVER_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_compile_options(${SERVER_NAME} PRIVATE "-O2;-Wall;-Wextra;-Wpedantic;-lboost_log_setup;-lboost_log;-lboost_program_options;")

find_library(SQLITE3_LIBRARY NAMES sqlite3)
find_package(Boost REQUIRED COMPONENTS system date_time unit_test_framework program_options log_setup log filesystem REQUIRED)

target_link_libraries (${SERVER_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${BOOST_PROGRAM_OPTIONS_LIBRARY}
    ${SQLITE3_LIBRARY}
    protocol_lib
    pthread
    project_warnings
    project_sanitizers
    messages_pb_lib
)

if (NOT WIN32)
    install(TARGETS ${SERVER_NAME} RUNTIME DESTINATION bin)
endif()

