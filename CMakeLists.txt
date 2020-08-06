cmake_minimum_required(VERSION 3.5)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(AppChat VERSION 0.7.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(AppChat VERSION 0.7.1)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(PROTOCOL_LIB_NAME protocol_lib)
set(LOGGER_LIB_NAME   logger_lib)
set(STORAGE_LIB_NAME  sqlite_db)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/lib)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system date_time unit_test_framework REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/lib/protocol
    ${CMAKE_SOURCE_DIR}/lib/log
    ${CMAKE_SOURCE_DIR}/lib/storage
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/client
)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/protocol)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/log)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/storage)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/server)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/client)

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/src/tests)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT scfocus@yandex.ru)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYFILE ${CMAKE_SOURCE_DIR}/Doxyfile)
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen")
    unset(DOXYFILE)
endif()
