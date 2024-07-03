cmake_minimum_required(VERSION 3.1.0)

project( robworld LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_options( -Wall -Wextra -Wno-unused-parameter -pedantic -fPIC -O2 )
add_link_options( -s )

set( OpenGL_GL_PREFERENCE GLVND )
find_package( Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED )
find_package( Threads REQUIRED )
find_package( OpenGL REQUIRED )

add_executable( ${PROJECT_NAME}
    src/main.cpp
    src/RobotWorld.cpp
    src/RobotBase.cpp
    src/RobotEPuck.cpp
    src/RobotThymio2.cpp
    src/RobotMarxbot.cpp
    src/BinaryData.cpp
    src/Connection.cpp
    src/LineProtocol.cpp
    src/GetProtocol.cpp
)

set( EXECUTABLE_OUTPUT_PATH "bin" )

target_include_directories( ${PROJECT_NAME} PUBLIC
    include
    ${ENKI_BUILD}/..
)

target_link_libraries( ${PROJECT_NAME} PUBLIC
    ${ENKI_BUILD}/viewer/libenkiviewer.a
    ${ENKI_BUILD}/enki/libenki.a
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL
    ${OPENGL_LIBRARIES}
    Threads::Threads
    jsoncpp
    crypto
)

if(WIN32)
target_link_libraries( ${PROJECT_NAME} PUBLIC
    wsock32
)
endif()
