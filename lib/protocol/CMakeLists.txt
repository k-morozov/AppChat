cmake_minimum_required(VERSION 3.5)

set(PROTOCOL_SRC
    protocol.h protocol.cpp
    command_table.h
    datetime.h datetime.cpp

    request/request.h
    request/input_request.h request/input_request.cpp
    request/transport_request.h request/transport_request.cpp
    request/control_request.h

    response/response.h
    response/input_response.h response/input_response.cpp
    response/transport_response.h
)

add_library(${PROTOCOL_LIB_NAME} SHARED STATIC
    ${PROTOCOL_SRC})

set_target_properties(${PROTOCOL_LIB_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic;-fPIC"
)

install(TARGETS ${PROTOCOL_LIB_NAME} DESTINATION /usr/local/bin)
install(TARGETS ${PROTOCOL_LIB_NAME} DESTINATION lib)
