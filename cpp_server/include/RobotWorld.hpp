#ifndef ROBOTWORLD_HPP
#define ROBOTWORLD_HPP

#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <iostream>
#include <thread>
#include <atomic>

#include "RobotBase.hpp"

namespace RobWorld
{
    class RobotWorld
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
            RobotWorld( std::string );
            ~RobotWorld();
            Enki::World* getWorld();
            double getWalls();
            void run();
            void stop();

        private:
            void dispatcher();
            void TRobot( int );
    };
}

#endif // ROBOTWORLD_HPP
