#ifndef ROBOTEPUCK_H
#define ROBOTEPUCK_H

#include<iostream>

#include <enki/robots/e-puck/EPuck.h>
#include "RobotBase.h"

class RobotEPuck : public RobotBase, public Enki::EPuck
{
    public:
        RobotEPuck( std::string, unsigned );
        ~RobotEPuck();
        virtual void controlStep( double dt );
        virtual void getSensors( JSON& resp );
        virtual void setLeds( double*, int );
        virtual unsigned char* getCameraImage( unsigned int* );

    private:
        double myProximitySensorValues[8];
        double myProximitySensorDistances[8];
        double myRingLed;
        unsigned char myCameraImage[240];
};

#endif // ROBOTEPUCK_H
