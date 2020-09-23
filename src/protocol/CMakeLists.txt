cmake_minimum_required(VERSION 3.5)

set(PROTOCOL_LIB_NAME protocol_lib)
add_library(${PROTOCOL_LIB_NAME} STATIC)

target_sources(${PROTOCOL_LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/protocol.h

    PRIVATE
        "include/protocol/command_table.h"
        "include/protocol/datetime.h" include/protocol/datetime.cpp
        "include/protocol/msgfactory.h" include/protocol/msgfactory.cpp
)

# fixme: replace this with better statement
target_include_directories(${PROTOCOL_LIB_NAME}
    PUBLIC
        ./include
    PRIVATE
        ./include/protocol/
)

set_target_properties(${PROTOCOL_LIB_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_compile_options(${PROTOCOL_LIB_NAME} PRIVATE "-O2;-fPIC")

target_link_libraries(${PROTOCOL_LIB_NAME}
    PRIVATE
        pthread
        project_sanitizers
)

find_package(Boost COMPONENTS date_time REQUIRED)

target_include_directories(${PROTOCOL_LIB_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

install(TARGETS ${PROTOCOL_LIB_NAME} DESTINATION /usr/local/lib)

add_library(messages_pb_lib STATIC include/protocol/proto/build/messages.pb.cc
include/protocol/proto/build/request.pb.cc 
include/protocol/proto/build/response.pb.cc)

set_target_properties(messages_pb_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(messages_pb_lib protobuf)

