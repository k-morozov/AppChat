cmake_minimum_required(VERSION 3.5)

set(SERVER_BIN      server)
set(DIR_CONNECTION  connection)
set(DIR_CHANNEL     channel)

set(CHANNELS_SRC
    ${DIR_CHANNEL}/iroom.h ${DIR_CHANNEL}/iroom.cpp
    ${DIR_CHANNEL}/channels_manager.h ${DIR_CHANNEL}/channels_manager.cpp
    ${DIR_CHANNEL}/channel.h ${DIR_CHANNEL}/channel.cpp
)
set(CONNECTIONS_SRC
    ${DIR_CONNECTION}/isubscriber.h ${DIR_CONNECTION}/isubscriber.cpp
    ${DIR_CONNECTION}/connection.h ${DIR_CONNECTION}/connection.cpp
    ${DIR_CONNECTION}/connection_manager.h ${DIR_CONNECTION}/connection_manager.cpp
)

set(SERVER_SOURCES
    startup_server.cpp
    server.h
    ${CONNECTIONS_SRC}
    ${CHANNELS_SRC}
)
add_executable(${SERVER_BIN}  ${SERVER_SOURCES})

set_target_properties(${SERVER_BIN} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_link_libraries (${SERVER_BIN}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${STORAGE_LIB_NAME}
    ${PROTOCOL_LIB_NAME}
    ${LOGGER_LIB_NAME}
)

install(TARGETS ${SERVER_BIN} RUNTIME DESTINATION bin)


