cmake_minimum_required(VERSION 3.5)
project(Logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# use if you have log4cplus
# find_library(LOGGER_CPLUS log4cplus)

add_subdirectory(log4cplus/)
set(LOGGER_SRC
    logger.h
    logger.cpp
)

#****************************************************************************************************
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

add_library(${LOGGER_LIB_NAME} STATIC ${LOGGER_SRC})
target_include_directories(${LOGGER_LIB_NAME} PUBLIC .)
target_link_libraries(${LOGGER_LIB_NAME}  log4cplus)

#install(TARGETS ${LOGGER_LIB_NAME})
