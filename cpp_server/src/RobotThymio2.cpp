#include "RobotThymio2.h"

RobotThymio2::RobotThymio2( std::string _name ) :
    RobotBase( _name, "thymio2" ), Thymio2(),
    myProximitySensorValues{},
    myProximitySensorDistances{},
    myGroundSensorValues{},
    myLeds{}
{
    std::cout << ">> Construyendo robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
}

RobotThymio2::~RobotThymio2()
{
    std::cout << ">> Destruyendo robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
}

void RobotThymio2::getSensors( JSON& resp )
{
    RobotBase::mtx_enki.lock();

    resp["proximitySensorValues"] = myProximitySensorValues;
    resp["proximitySensorDistances"] = myProximitySensorDistances;
    resp["groundSensorValues"] = myGroundSensorValues;

    RobotBase::mtx_enki.unlock();
}

void RobotThymio2::setLeds( double* leds, int nleds )
{
    RobotBase::mtx_enki.lock();

    for( int led=0; led < int( sizeof( myLeds )/sizeof( myLeds[0] ) ) && led < nleds; led++ )
        myLeds[led] = leds[led];

    RobotBase::mtx_enki.unlock();
}

unsigned char* RobotThymio2::getCameraImage( unsigned int* len )
{
    *len = 0;
    return NULL;
}

void RobotThymio2::controlStep( double dt )
{
    RobotBase::mtx_enki.lock();

    // hacia el robot
    for( int led=0; led < int( sizeof( myLeds )/sizeof( myLeds[0] ) ); led++ )
        Thymio2::setLedIntensity( (LedIndex)led, myLeds[led] );

    // desde el robot
    myProximitySensorValues[0] = Thymio2::infraredSensor0.getValue();
    myProximitySensorValues[1] = Thymio2::infraredSensor1.getValue();
    myProximitySensorValues[2] = Thymio2::infraredSensor2.getValue();
    myProximitySensorValues[3] = Thymio2::infraredSensor3.getValue();
    myProximitySensorValues[4] = Thymio2::infraredSensor4.getValue();
    myProximitySensorValues[5] = Thymio2::infraredSensor5.getValue();
    myProximitySensorValues[6] = Thymio2::infraredSensor6.getValue();

    myProximitySensorDistances[0] = Thymio2::infraredSensor0.getDist();
    myProximitySensorDistances[1] = Thymio2::infraredSensor1.getDist();
    myProximitySensorDistances[2] = Thymio2::infraredSensor2.getDist();
    myProximitySensorDistances[3] = Thymio2::infraredSensor3.getDist();
    myProximitySensorDistances[4] = Thymio2::infraredSensor4.getDist();
    myProximitySensorDistances[5] = Thymio2::infraredSensor5.getDist();
    myProximitySensorDistances[6] = Thymio2::infraredSensor6.getDist();

    myGroundSensorValues[0] = Thymio2::groundSensor0.getValue();
    myGroundSensorValues[1] = Thymio2::groundSensor1.getValue();

    RobotBase::mtx_enki.unlock();

    RobotBase::myControlStep( this );
    Thymio2::controlStep( dt );
}
