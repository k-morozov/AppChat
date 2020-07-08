cmake_minimum_required(VERSION 3.5)
project(Protocol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOCOL_SRC
    protocol.h protocol.cpp
    command_table.h

    request/request.h
    request/input_request.h request/input_request.cpp
    request/transport_request.h request/transport_request.cpp
    request/control_request.h

    response/response.h
    response/input_response.h response/input_response.cpp
    response/transport_response.h

)
#****************************************************************************************************

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_library(${PROTOCOL_LIB_NAME} ${PROTOCOL_SRC})
target_include_directories(${PROTOCOL_LIB_NAME} PUBLIC .)
