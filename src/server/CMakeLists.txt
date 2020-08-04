cmake_minimum_required(VERSION 3.5)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(Server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(Server VERSION 0.0.1)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

add_definitions(-DBOOST_STACKTRACE_USE_BACKTRACE)

set(SERVER_BIN      server)
set(DIR_CONNECTION  connection)
set(DIR_CHANNEL     channel)

#****************************************************************************************************
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

#****************************************************************************************************
set(SERVER_SOURCES
    startup_server.cpp
    server.h
    ${CONNECTIONS_SRC}
    ${CHANNELS_SRC}
)
add_executable(${SERVER_BIN}  ${SERVER_SOURCES})
target_include_directories(${SERVER_BIN} PUBLIC .)
#****************************************************************************************************
find_package(Boost COMPONENTS system date_time REQUIRED)

set_target_properties(${SERVER_BIN} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

add_dependencies(${SERVER_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${STORAGE_LIB_NAME} ${PROTOCOL_LIB_NAME} ${LOGGER_LIB_NAME}
    ${BACKTRACE_LIB_NAME} ${TRACEDEXCEPTION_LIB_NAME})
target_link_libraries (${SERVER_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${STORAGE_LIB_NAME} ${PROTOCOL_LIB_NAME} ${LOGGER_LIB_NAME}
    ${BACKTRACE_LIB_NAME} ${TRACEDEXCEPTION_LIB_NAME})

# install to bin folder our binaries
install(TARGETS ${SERVER_BIN} RUNTIME DESTINATION bin)

# generate .deb package
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT scfocus@yandex.ru)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# include will generate proper code for .deb generation
include(CPack)

target_include_directories(${SERVER_BIN}    PUBLIC ${SRC}/server})
