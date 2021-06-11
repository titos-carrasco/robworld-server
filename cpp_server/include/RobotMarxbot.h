#ifndef ROBOTMARXBOT_H
#define ROBOTMARXBOT_H

#include<iostream>

#include <enki/robots/marxbot/Marxbot.h>
#include "RobotBase.h"

class RobotMarxbot : public RobotBase, public Enki::Marxbot
{
    public:
        RobotMarxbot( std::string );
        ~RobotMarxbot();
        virtual void controlStep( double dt );
        virtual void getSensors( JSON& resp );
        virtual void setLeds( double*, int );
        virtual unsigned char* getCameraImage( unsigned int* );

    private:
        double myVirtualBumpers[24];
};

#endif // ROBOTMARXBOT_H
