cmake_minimum_required(VERSION 3.5)

set(PROTOCOL_LIB_NAME protocol_lib)
add_library(${PROTOCOL_LIB_NAME} STATIC)

target_sources(${PROTOCOL_LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/protocol.h

    PRIVATE
        "include/protocol/command_table.h"
        "include/protocol/datetime.h" datetime.cpp

#        "include/protocol/request/request.h"
#        "include/protocol/request/input_request.h" request/input_request.cpp
#        "include/protocol/request/transport_request.h" request/transport_request.cpp
#        "include/protocol/request/control_request.h"

#        "include/protocol/response/response.h"
#        "include/protocol/response/input_response.h" response/input_response.cpp
#        "include/protocol/response/transport_response.h"
        "include/protocol/msgfactory.h" include/protocol/msgfactory.cpp
)

# fixme: replace this with better statement
target_include_directories(${PROTOCOL_LIB_NAME}
    PUBLIC
        ./include
    PRIVATE
        ./include/protocol
)

set_target_properties(${PROTOCOL_LIB_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_compile_options(${PROTOCOL_LIB_NAME} PRIVATE "-O2;-Wall;-Wextra;-Wpedantic;-fPIC")

target_link_libraries(${PROTOCOL_LIB_NAME}
    PRIVATE
        pthread
        project_warnings
        project_sanitizers
)

find_package(Boost COMPONENTS date_time REQUIRED)

target_include_directories(${PROTOCOL_LIB_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

install(TARGETS ${PROTOCOL_LIB_NAME} DESTINATION /usr/local/lib)

add_library(messages_pb_lib STATIC include/protocol/messages.pb.cc)
target_link_libraries(messages_pb_lib protobuf)
set_target_properties(messages_pb_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
