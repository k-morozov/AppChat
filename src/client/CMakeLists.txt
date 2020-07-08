cmake_minimum_required(VERSION 3.5)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

#****************************************************************************************************
set(Control_dir control)
set(Client_dir client)
set(GUI_dir gui)
#****************************************************************************************************
set(CLIENT_SOURCES
    startup_client.cpp
    ${Client_dir}/client.h  ${Client_dir}/client.cpp
    ${GUI_dir}/mainwindow.h ${GUI_dir}/mainwindow.cpp
    ${GUI_dir}/mainwindow.ui
    ${Control_dir}/control.h ${Control_dir}/control.cpp
)

#****************************************************************************************************
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package (Threads)

add_executable(${CLIENT_BIN}  ${CLIENT_SOURCES})
target_include_directories(${CLIENT_BIN} PUBLIC .)

#****************************************************************************************************
find_package(Boost COMPONENTS system date_time program_options regex REQUIRED)
target_link_libraries (${CLIENT_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} Qt5::Widgets ${PROTOCOL_LIB_NAME})

