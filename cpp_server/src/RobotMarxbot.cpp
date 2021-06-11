#include "RobotMarxbot.h"

RobotMarxbot::RobotMarxbot( std::string _name ) :
    RobotBase( _name, "marxbot" ), Marxbot(),
    myVirtualBumpers{}
{
    std::cout << ">> Construyendo robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
}

RobotMarxbot::~RobotMarxbot()
{
    std::cout << ">> Destruyendo robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
}

void RobotMarxbot::getSensors( JSON& resp )
{
    resp["virtualBumpers"] = myVirtualBumpers;
}

void RobotMarxbot::setLeds( double* leds, int nleds )
{
}

unsigned char* RobotMarxbot::getCameraImage( unsigned int* len )
{
    *len = 0;
    return NULL;
}

void RobotMarxbot::controlStep( double dt )
{
    RobotBase::mtx_enki.lock();

    // hacia el robot

    // desde el robot
    int n = int( sizeof( myVirtualBumpers )/sizeof( myVirtualBumpers[0] ) );
    for( int i=0; i < n; i++ )
        myVirtualBumpers[i] = Marxbot::getVirtualBumper( i );

    RobotBase::myControlStep( this );

    RobotBase::mtx_enki.unlock();

    Marxbot::controlStep( dt );
}