cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(AppChat VERSION 0.7.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(AppChat VERSION 0.7.1)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/src)
set(PROTOCOL_LIB_NAME protocol_lib)

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    message("DEBUG mode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug/)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -Og -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=leak")
else ()
    message("Release mode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release/)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYFILE ${CMAKE_SOURCE_DIR}/Doxyfile)
        add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen")
        unset(DOXYFILE)
    endif()
endif()

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/src/protocol)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/server)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/client)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/tests)
