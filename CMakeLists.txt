cmake_minimum_required(VERSION 3.5)
project(AppChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

#****************************************************************************************************
set(DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DIR_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(PROTOCOL_LIB_NAME        protocol_lib)
set(LOGGER_LIB_NAME          logger_lib)
set(STORAGE_LIB_NAME         sqlite_db)
set(BACKTRACE_LIB_NAME       backtrace)
set(TRACEDEXCEPTION_LIB_NAME tracedexception_lib)

#****************************************************************************************************
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/lib)
#****************************************************************************************************
enable_testing()

add_subdirectory(${DIR_LIB}/protocol)
add_subdirectory(${DIR_LIB}/log)
add_subdirectory(${DIR_LIB}/storage)

add_subdirectory(${DIR_SRC}/server)
add_subdirectory(${DIR_SRC}/client)
add_subdirectory(${DIR_SRC}/tests)

#****************************************************************************************************
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYFILE ${CMAKE_SOURCE_DIR}/Doxyfile)
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen")
    unset(DOXYFILE)
endif()

