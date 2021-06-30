#ifndef ROBOTMARXBOT_HPP
#define ROBOTMARXBOT_HPP

#include "RobotBase.hpp"
#include "Connection.hpp"

#include <iostream>

#include <enki/robots/marxbot/Marxbot.h>

namespace RobWorld
{
    class RobotMarxbot : public RobotBase, public Enki::Marxbot
    {
        public:
            RobotMarxbot( std::string );
            ~RobotMarxbot();
            virtual void controlStep( double dt );
            virtual void getSensors( Json::Value& resp );
            virtual void setLeds( const double[], int );
            virtual BinaryData* getCameraImage();

        private:
            double myVirtualBumpers[24];
    };
}

#endif // ROBOTMARXBOT_HPP
