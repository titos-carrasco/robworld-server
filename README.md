<h1>RobWorld</h1>

Simulador de entorno de robots en C++ utilizando [ENKI Robot Simulator](https://github.com/enki-community/enki)

![](images/img-01.png "") ![](images/img-02.png "") ![](images/img-03.png "")

![](images/img-04.png "") ![](images/img-05.png "") ![](images/img-06.png "")

![](images/img-07.png "") ![](images/img-08.png "")


El acceso al simulador es a través de TCP/IP, intercambiando mensajes en formato JSON y utilizando las librerias:

- [robworld-client-java](https://github.com/titos-carrasco/robworld-client-java)
- [robworld-client-python](https://github.com/titos-carrasco/robworld-client-python)


</br>

<h1>MSYS2 MINGW64</h1>

    $ pacman -S make
    $ pacman -S mingw-w64-x86_64-make
    $ pacman -S mingw-w64-x86_64-cmake
    $ pacman -S mingw-w64-x86_64-gcc
    $ pacman -S mingw-w64-x86_64-gcc-libs
    $ pacman -S mingw-w64-x86_64-SDL2
    $ pacman -S mingw-w64-x86_64-qt5-base
    $ pacman -S mingw-w64-x86_64-qt5-imageformats
    $ pacman -S mingw-w64-x86_64-ntldd

</br>

<h1>MSYS2 MINGW32</h1>

    $ pacman -S make
    $ pacman -S mingw-w32-i686-make
    $ pacman -S mingw-w64-i686-cmake
    $ pacman -S mingw-w64-i686-gcc
    $ pacman -S mingw-w64-i686-gcc-libs
    $ pacman -S mingw-w64-i686-SDL2
    $ pacman -S mingw-w64-i686-qt5-base
    $ pacman -S mingw-w64-i686-qt5-imageformats
    $ pacman -S mingw-w64-i686-ntldd

</br>

<h1>Librería ENKI</h1>

    $ git clone https://github.com/enki-community/enki.git
    $ cd enki/

**MINGW32**

    $ mkdir build32
    $ cd build32
    $ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=GLVND ..
    $ cmake --build . --config Release
    $ ./examples/playground/enkiplayground

**MINGW64**

    $ mkdir build64
    $ cd build64
    $ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOpenGL_GL_PREFERENCE=GLVND ..
    $ cmake --build . --config Release
    $ ./examples/playground/enkiplayground

<br/>

<h1>RobWorld Server</h1>

    $ git clone https://github.com/titos-carrasco/robworld-server.git
    $ cd robworld-server/cpp_server

**MINGW32**

    $ mkdir build32
    $ cd build32
    $ cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release -D OpenGL_GL_PREFERENCE=GLVND -D ENKI_BUILD=E:/enki/build32 ..
    $ cmake --build . --config Release 
    $ ./bin/robworld
    $ cd ../..
    $ ./mkdist.sh 32

**MINGW64**

    $ mkdir build64
    $ cd build64
    $ cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release -D OpenGL_GL_PREFERENCE=GLVND -D ENKI_BUILD=E:/enki/build64 ..
    $ cmake --build . --config Release 
    $ ./bin/robworld
    $ cd ../..
    $ ./mkdist.sh 64
