#ifndef ROBOTTHYMIO2_HPP
#define ROBOTTHYMIO2_HPP

#include <iostream>

#include <enki/robots/thymio2/Thymio2.h>

#include "RobotBase.hpp"
#include "Connection.hpp"

namespace RobWorld
{
    class RobotThymio2 : public RobotBase, public Enki::Thymio2
    {
        public:
            RobotThymio2( std::string );
            ~RobotThymio2();
            virtual void controlStep( double dt );
            virtual void getSensors( Json::Value& resp );
            virtual void setLeds( const double[], int );
            virtual BinaryData* getCameraImage();

        private:
            double myProximitySensorValues[7];
            double myProximitySensorDistances[7];
            double myGroundSensorValues[2];
            double myLeds[25];
    };
}

#endif // ROBOTTHYMIO2_HPP
