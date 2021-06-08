#ifndef MYPLAYGROUND_H
#define MYPLAYGROUND_H

#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif // WIN32

#include <iostream>
#include <thread>
#include <map>
#include <atomic>

#include "RobotThymio2.h"
#include "RobotEPuck.h"

#include <enki/PhysicalEngine.h>
#include <viewer/Viewer.h>

class MyPlayground
{
    private:
        Enki::World* world;
        double walls;
        std::thread* tDispatcher;
        std::atomic_bool tDispatcherRunning;
        int srv_sock;
        struct ::sockaddr_in srv_address;
        std::map<std::string, RobotBase*> robots;

    public:
        MyPlayground( std::string );
        ~MyPlayground();
        Enki::World* getWorld();
        double getWalls();
        void run();
        void stop();

    private:
        void dispatcher();
        void TRobot( int );
};

#endif // MYPLAYGROUND_H
