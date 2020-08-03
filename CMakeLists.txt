cmake_minimum_required(VERSION 3.5)
project(AppChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

#****************************************************************************************************
set(SRC     src)
set(LIB     lib)

set(PROTOCOL_LIB_NAME        protocol_lib)
set(LOGGER_LIB_NAME          logger_lib)
set(STORAGE_LIB_NAME         sqlite_db)
set(BACKTRACE_LIB_NAME       backtrace)
set(TRACEDEXCEPTION_LIB_NAME tracedexception_lib)

#****************************************************************************************************
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#****************************************************************************************************
enable_testing()

add_subdirectory(${SRC}/server)
add_subdirectory(${SRC}/client)
add_subdirectory(${LIB}/protocol)
add_subdirectory(${LIB}/log)
add_subdirectory(${LIB}/storage)
add_subdirectory(${LIB}/libbacktrace)
add_subdirectory(${LIB}/traced_exception)
add_subdirectory(${SRC}/tests)

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

