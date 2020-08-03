cmake_minimum_required(VERSION 3.5)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#****************************************************************************************************
set(CLIENT_BIN  client)
set(Control_dir control)
set(Client_dir client)
set(GUI_dir gui)
#****************************************************************************************************
set(CLIENT_SOURCES
    startup_client.cpp
	${Client_dir}/client.h  ${Client_dir}/client.cpp
	${Control_dir}/control.h ${Control_dir}/control.cpp
	${GUI_dir}/qml.qrc
)

#****************************************************************************************************
find_package(Qt5 COMPONENTS Core Gui Qml Quick Widgets REQUIRED)
find_package (Threads)

add_executable(${CLIENT_BIN}  ${CLIENT_SOURCES})
target_include_directories(${CLIENT_BIN} PUBLIC .)

#****************************************************************************************************
find_package(Boost COMPONENTS system date_time program_options regex REQUIRED)

set_target_properties(${CLIENT_BIN} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_link_libraries (${CLIENT_BIN} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${PROTOCOL_LIB_NAME}
	Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Widgets)

# install to bin folder our binaries
install(TARGETS ${CLIENT_BIN} RUNTIME DESTINATION bin)

# generate .deb package
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT scfocus@yandex.ru)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# include will generate proper code for .deb generation
include(CPack)

target_include_directories(${CLIENT_BIN}    PUBLIC ${SRC}/client)
