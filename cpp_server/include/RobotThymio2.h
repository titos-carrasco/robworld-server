#ifndef ROBOTTHYMIO2_H
#define ROBOTTHYMIO2_H

#include<iostream>

#include <enki/robots/thymio2/Thymio2.h>
#include "RobotBase.h"

class RobotThymio2 : public RobotBase, public Enki::Thymio2
{
    public:
        RobotThymio2( std::string );
        ~RobotThymio2();
        virtual void controlStep( double dt );
        virtual void getSensors( JSON& resp );
        virtual void setLeds( double*, int );
        virtual unsigned char* getCameraImage( unsigned int* );

    private:
        double myProximitySensorValues[7];
        double myProximitySensorDistances[7];
        double myGroundSensorValues[2];
        double myLeds[25];
};

#endif // ROBOTTHYMIO2_H
