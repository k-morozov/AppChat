cmake_minimum_required(VERSION 3.5)

option(CHECK_SANITIZE "enable sanitizers checks on debug mode" OFF)
option(CHECK_DOXYGEN "enable doc generation" OFF)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(AppChat VERSION 0.7.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(AppChat VERSION 0.7.1)
endif ()

# Adding sanitizers
add_library(project_sanitizers INTERFACE)

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    message("DEBUG mode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug/)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -Og -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1")
    if (CHECK_SANITIZE)
        include(${CMAKE_SOURCE_DIR}/cmake/Sanitizers.cmake)
        message(ERROR "Check sanitize")
        enable_sanitizers(project_sanitizers)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    endif()
else ()
    message("Release mode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release/)
    if (${CHECK_DOXYGEN} STREQUAL ON)
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
endif()

include_directories(${Protobuf_INCLUDE_DIRS})

include(${CMAKE_SOURCE_DIR}/cmake/ProjectWarnings.cmake)
add_library(project_warnings INTERFACE)
enable_warnings(project_warnings)

enable_testing()

# Build protocol_lib
add_subdirectory(${CMAKE_SOURCE_DIR}/src/protocol)

# Build appchat-server
if (NOT WIN32)
#    add_subdirectory(${CMAKE_SOURCE_DIR}/src/server)
endif()

# Build appchat-client
add_subdirectory(${CMAKE_SOURCE_DIR}/src/client)

# build tests
#add_subdirectory(${CMAKE_SOURCE_DIR}/src/tests)

if (NOT WIN32)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_CONTACT scfocus@yandex.ru)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    include(CPack)
endif()
