cmake_minimum_required(VERSION 3.5)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CLIENT_BIN  client)
set(Control_dir control)
set(Client_dir client)
set(GUI_dir gui)

set(CLIENT_SOURCES
    startup_client.cpp
    ${Client_dir}/client.h  ${Client_dir}/client.cpp
    ${GUI_dir}/mainwindow.h ${GUI_dir}/mainwindow.cpp
    ${GUI_dir}/mainwindow.ui
    ${Control_dir}/control.h ${Control_dir}/control.cpp
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package (Threads)

add_executable(${CLIENT_BIN}  ${CLIENT_SOURCES})

find_package(Boost COMPONENTS system date_time REQUIRED)

set_target_properties(${CLIENT_BIN} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_link_libraries (${CLIENT_BIN}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    Qt5::Widgets
    ${PROTOCOL_LIB_NAME}
)

install(TARGETS ${CLIENT_BIN} RUNTIME DESTINATION bin)
