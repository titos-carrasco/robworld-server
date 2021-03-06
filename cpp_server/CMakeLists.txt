# Utilizar mingw64 en Windows para generar las librerias de ENKI
# git clone https://github.com/enki-community/enki.git
# cd enki
# mkdir build
# cd build
# cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=GLVND ..
# cmake --build . --config Release
# ./examples/playground/enkiplayground

# mkdir build
# cd build
# cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=GLVND ..
# cmake --build . --config Release
# ./bin/robworld

# mingw64
# mkdir build64
# cd build64
# cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=GLVND -DX=64 ..

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

if(WIN32)
    target_include_directories( ${PROJECT_NAME} PUBLIC
        include
        C:/Users/titos/Documents/Software/enki
    )

    target_link_libraries( ${PROJECT_NAME} PUBLIC
        C:/Users/titos/Documents/Software/enki/build${X}/enki/libenki.a
        C:/Users/titos/Documents/Software/enki/build${X}/viewer/libenkiviewer.a
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL
        ${OPENGL_LIBRARIES}
        Threads::Threads
        jsoncpp
        wsock32
    )
else()
    target_include_directories( ${PROJECT_NAME} PUBLIC
        include
        /mnt/sda5/roberto/Software/enki
    )

    target_link_libraries( ${PROJECT_NAME} PUBLIC
        /mnt/sda5/roberto/Software/enki/build/viewer/libenkiviewer.a
        /mnt/sda5/roberto/Software/enki/build/enki/libenki.a
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL
        ${OPENGL_LIBRARIES}
        Threads::Threads
        jsoncpp
        crypto
    )
endif()
