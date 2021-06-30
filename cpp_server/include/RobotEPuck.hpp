#ifndef ROBOTEPUCK_HPP
#define ROBOTEPUCK_HPP

#include "RobotBase.hpp"
#include "Connection.hpp"

#include <iostream>

#include <enki/robots/e-puck/EPuck.h>

namespace RobWorld
{
    class RobotEPuck : public RobotBase, public Enki::EPuck
    {
        public:
            RobotEPuck( std::string, unsigned );
            ~RobotEPuck();
            virtual void controlStep( double dt );
            virtual void getSensors( Json::Value& resp );
            virtual void setLeds( const double[], int );
            virtual BinaryData* getCameraImage();

        private:
            double myProximitySensorValues[8];
            double myProximitySensorDistances[8];
            double myRingLed;
            unsigned char myCameraImage[240];
    };
}

#endif // ROBOTEPUCK_HPP
