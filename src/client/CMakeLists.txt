cmake_minimum_required(VERSION 3.5)

set(CLIENT_NAME appchat-client)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CLIENT_BIN  client)
set(Control_dir control)
set(Client_dir client)
set(GUI_dir gui)

set(CMAKE_CXX_FLAGS "-lboost_program_options")

set(CLIENT_SOURCES
    startup_client.cpp
    ${Client_dir}/client.h
    ${Client_dir}/client.cpp
    ${GUI_dir}/mainwindow.h
    ${GUI_dir}/mainwindow.cpp
    ${GUI_dir}/mainwindow.ui
    ${Control_dir}/control.h
    ${Control_dir}/control.cpp
)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package (Threads)

add_executable(${CLIENT_NAME}  ${CLIENT_SOURCES})

find_package(Boost COMPONENTS system date_time program_options REQUIRED)

set_target_properties(${CLIENT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_link_libraries (${CLIENT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${BOOST_PROGRAM_OPTIONS_LIBRARY}
    Qt5::Widgets
    ${PROTOCOL_LIB_NAME}
)

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    target_link_libraries(${CLIENT_NAME} -lws2_32)
endif (WIN32)

if (NOT WIN32)
    install(TARGETS ${CLIENT_NAME} RUNTIME DESTINATION bin)
endif()
