cmake_minimum_required(VERSION 3.5)

set(CLIENT_NAME appchat-client)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CLIENT_BIN  client)
set(Control_dir control)
set(Client_dir client)
set(GUI_dir gui)

set(CLIENT_SOURCES
    startup_client.cpp
    ${Client_dir}/client.h
    ${Client_dir}/client.cpp
    ${GUI_dir}/mainwindow.h
    ${GUI_dir}/mainwindow.cpp
    ${GUI_dir}/chatwindow.cpp
    ${GUI_dir}/chatwindow.h
    ${GUI_dir}/mainwindow.ui
    ${GUI_dir}/chatwindow.ui
    ${Control_dir}/control.h
    ${Control_dir}/control.cpp
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package (Threads)

add_executable(${CLIENT_NAME}  ${CLIENT_SOURCES})

#add_library(messages_pb_lib STATIC messages.pb.cc)
#target_link_libraries(messages_pb_lib protobuf)
#set_target_properties(messages_pb_lib PROPERTIES
#        CXX_STANDARD 17
#        CXX_STANDARD_REQUIRED ON
#)

find_package(Boost COMPONENTS system date_time REQUIRED)

set_target_properties(${CLIENT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_include_directories(${CLIENT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (${CLIENT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    Qt5::Widgets
    protocol_lib
    project_warnings
    project_sanitizers
    messages_pb_lib
)

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    target_link_libraries(${CLIENT_NAME} -lws2_32)
endif (WIN32)

if (NOT WIN32)
    install(TARGETS ${CLIENT_NAME} RUNTIME DESTINATION bin)
endif()



