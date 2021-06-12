#ifndef ROBOTBASE_H
#define ROBOTBASE_H

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
#include <atomic>
#include <mutex>
#include <vector>
#include <thread>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <enki/robots/DifferentialWheeled.h>
#include <enki/PhysicalEngine.h>

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>

class RobotBase
{
    private:
        std::atomic_bool running;
        std::mutex mtx;
        double mypos[2];
        double myspeed[2];
        char in_buffer[512];

    protected:
        std::mutex mtx_enki;
        std::string name;
        std::string tipo;

    public:
        RobotBase( std::string, std::string );
        virtual ~RobotBase();
        void run( int );
        void stop();

    private:
        int readline( int );
        bool sendline( int, std::string );
        bool sendbytes( int, unsigned char*, unsigned int );

    protected:
        void myControlStep( Enki::DifferentialWheeled* );
        virtual void getSensors( Json::Value& resp ) = 0;
        virtual void setLeds( double*, int ) = 0;
        virtual unsigned char* getCameraImage( unsigned int* ) = 0;
};

#endif // ROBOTBASE_H
