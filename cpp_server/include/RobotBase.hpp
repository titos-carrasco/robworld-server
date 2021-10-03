#ifndef ROBOTBASE_HPP
#define ROBOTBASE_HPP

#include "Connection.hpp"

#ifdef WIN32
#include <json/json.h>
#include <json/writer.h>
#else
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#endif

#include <iostream>
#include <atomic>
#include <mutex>

#include <enki/robots/DifferentialWheeled.h>

namespace RobWorld
{
    class RobotBase
    {
        private:
            std::atomic_bool running;
            std::mutex mtx;
            double mypos[2];
            double myspeed[2];

        protected:
            std::mutex mtx_enki;
            std::string name;
            std::string tipo;
            double myAngleRad;

        public:
            RobotBase( std::string, std::string );
            virtual ~RobotBase();
            void run( Connection& );
            void stop();

        protected:
            void myControlStep( Enki::DifferentialWheeled* );
            virtual void getSensors( Json::Value& resp ) = 0;
            virtual void setLeds( const double[], int ) = 0;
            virtual BinaryData* getCameraImage() = 0;
    };

}

#endif // ROBOTBASE_HPP
