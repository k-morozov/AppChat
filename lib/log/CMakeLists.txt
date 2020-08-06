cmake_minimum_required(VERSION 3.5)

set(LOGGER_SRC
    logger.h
    logger.cpp
)

add_library(${LOGGER_LIB_NAME}
    ${LOGGER_SRC})

set_target_properties(${LOGGER_LIB_NAME} PROPERTIES
    CMAKE_CXX_STANDARD 17
    CMAKE_CXX_STANDARD_REQUIRED ON
    LINK_LIBRARIES pthread
    COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic;-fPIC"
)

add_subdirectory(log4cplus/)
target_link_libraries(${LOGGER_LIB_NAME} log4cplus)

install(TARGETS ${LOGGER_LIB_NAME} DESTINATION /usr/local/lib)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/logger.h DESTINATION /usr/local/bin/)
