cmake_minimum_required(VERSION 3.5)

set(STORAGE_SRC
     database.h
     database.cpp
)

add_library(${STORAGE_LIB_NAME} SHARED STATIC
    ${STORAGE_SRC})

set_target_properties(${STORAGE_LIB_NAME} PROPERTIES
    CMAKE_CXX_STANDARD 17
    CMAKE_CXX_STANDARD_REQUIRED ON
)

find_library(SQLITE3_LIBRARY NAMES sqlite3)
find_package(Boost REQUIRED)
target_link_libraries (${STORAGE_LIB_NAME} ${PROTOCOL_LIB_NAME} ${LOGGER_LIB_NAME} ${SQLITE3_LIBRARY} ${Boost_LIBRARIES})

install(TARGETS ${STORAGE_LIB_NAME} DESTINATION /usr/local/bin)
install(TARGETS ${STORAGE_LIB_NAME} DESTINATION lib)
