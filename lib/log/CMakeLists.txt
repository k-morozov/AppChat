cmake_minimum_required(VERSION 3.5)
project(Logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(LOGGER_CPLUS log4cplus)

#add_subdirectory(log4cplus/)
set(LOGGER_SRC
    logger.h
    logger.cpp
)
#****************************************************************************************************
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

add_library(logger_lib  ${LOGGER_SRC})
target_include_directories(logger_lib PUBLIC .)
target_link_libraries(logger_lib ${LOGGER_CPLUS})
