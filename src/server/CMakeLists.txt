cmake_minimum_required(VERSION 3.5)

set(SERVER_NAME appchat-server)

set(Boost_USE_STATIC_LIBS ON)
#add_definitions(-DBOOST_LOG_DYN_LINK)

set(DIR_CONNECTION  connection)
set(DIR_CHANNEL     channel)
set(DIR_LOG         log)
set(DIR_STORAGE     storage)
set(DIR_PROTOCOL    protocol)

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

add_executable(${SERVER_NAME}  ${SERVER_SOURCES})

set_target_properties(${SERVER_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic;-lboost_log_setup;-lboost_log;"
)

find_library(SQLITE3_LIBRARY NAMES sqlite3)
find_package(Boost REQUIRED COMPONENTS system date_time unit_test_framework log_setup log filesystem REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${PROTOCOL_LIB_NAME}
)

target_link_libraries (${SERVER_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
#    ${Boost_LOG_LIBRARY}
#    ${Boost_LOG_SETUP_LIBRARY}
    ${SQLITE3_LIBRARY}
    ${PROTOCOL_LIB_NAME}
)

install(TARGETS ${SERVER_NAME} RUNTIME DESTINATION bin)
set(CPACK_PACKAGE_NAME ${SERVER_NAME})
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT scfocus@yandex.ru)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)


