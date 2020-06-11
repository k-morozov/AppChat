cmake_minimum_required(VERSION 3.5)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    #message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} doesn't have C++17 support.")
endif()
message(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(PROJECT Chat)
set(SERVER server)
set(CLIENT client)

set(SRC src/)
set(Server ${SRC}server/)
set(Cleint ${SRC}client/)
set(Protocol ${SRC}protocol/)

project(${PROJECT} LANGUAGES CXX)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

set(SERVER_SOURCES ${Server}startup_server.cpp
    ${Server}server.h
    ${Server}session.h
    ${Protocol}protocol.h ${Protocol}protocol.cpp
)

set(CLIENT_SOURCES ${Cleint}startup_client.cpp
    ${Protocol}protocol.h
    ${Protocol}protocol.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# add projects
add_executable(${SERVER} ${SERVER_SOURCES})
add_executable(${CLIENT} ${CLIENT_SOURCES})

set_target_properties(${SERVER} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        #COMPILE_OPTIONS "-O2;-Wall;-Wextra;-Wpedantic"
)

set_target_properties(${CLIENT} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        #COMPILE_OPTIONS "-O2;-Wall;-Wextra;-Wpedantic"
)

find_package(Boost COMPONENTS system date_time filesystem program_options regex thread graph REQUIRED)
find_package (Threads)

target_link_libraries (${SERVER} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
target_link_libraries (${CLIENT} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

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
