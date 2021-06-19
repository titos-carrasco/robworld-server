#include "RobotEPuck.hpp"

namespace RobWorld
{
    RobotEPuck::RobotEPuck( std::string _name, unsigned _caps ) :
        RobotBase( _name, "epuck" ), EPuck( _caps ),
        myProximitySensorValues{},
        myProximitySensorDistances{},
        myRingLed(0),
        myCameraImage{}
    {
        std::cout << ">> Construyendo robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "'" << std::endl;
    }

    RobotEPuck::~RobotEPuck()
    {
        std::cout << ">> Destruyendo robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "'" << std::endl;
    }

    void RobotEPuck::getSensors( Json::Value& resp )
    {
        resp["proximitySensorValues"] = Json::arrayValue;
        for( unsigned int i = 0; i<sizeof(myProximitySensorValues)/sizeof(myProximitySensorValues[0]); i++ ) resp["proximitySensorValues"].append( myProximitySensorValues[i] );

        resp["proximitySensorDistances"] = Json::arrayValue;
        for( unsigned int i = 0; i<sizeof(myProximitySensorDistances)/sizeof(myProximitySensorDistances[0]); i++ ) resp["proximitySensorDistances"].append( myProximitySensorDistances[i] );
    }

    void RobotEPuck::setLeds( const double leds[], int len )
    {
        myRingLed = leds[0];
    }

    BinaryData* RobotEPuck::getCameraImage()
    {
        // la BinaryData debe ser liberada en el invocador
        unsigned int len = sizeof( myCameraImage )/sizeof( myCameraImage[0] );
        unsigned char *img = new unsigned char[len];
        memcpy( img, myCameraImage, len );
        return new BinaryData( img, len );
    }

    void RobotEPuck::controlStep( double dt )
    {
        RobotBase::mtx_enki.lock();

        // hacia el robot
        EPuck::setLedRing( myRingLed );

        // desde el robot
        myProximitySensorValues[0] = EPuck::infraredSensor0.getValue();
        myProximitySensorValues[1] = EPuck::infraredSensor1.getValue();
        myProximitySensorValues[2] = EPuck::infraredSensor2.getValue();
        myProximitySensorValues[3] = EPuck::infraredSensor3.getValue();
        myProximitySensorValues[4] = EPuck::infraredSensor4.getValue();
        myProximitySensorValues[5] = EPuck::infraredSensor5.getValue();
        myProximitySensorValues[6] = EPuck::infraredSensor6.getValue();
        myProximitySensorValues[7] = EPuck::infraredSensor7.getValue();

        myProximitySensorDistances[0] = EPuck::infraredSensor0.getDist();
        myProximitySensorDistances[1] = EPuck::infraredSensor1.getDist();
        myProximitySensorDistances[2] = EPuck::infraredSensor2.getDist();
        myProximitySensorDistances[3] = EPuck::infraredSensor3.getDist();
        myProximitySensorDistances[4] = EPuck::infraredSensor4.getDist();
        myProximitySensorDistances[5] = EPuck::infraredSensor5.getDist();
        myProximitySensorDistances[6] = EPuck::infraredSensor6.getDist();
        myProximitySensorDistances[7] = EPuck::infraredSensor7.getDist();

        int n = 0;
        for(size_t i = 0; i < camera.image.size(); i++ )
        {
            Enki::Color c = camera.image[i];
            myCameraImage[n++] = int( c.r()*255 );
            myCameraImage[n++] = int( c.g()*255 );
            myCameraImage[n++] = int( c.b()*255 );
            myCameraImage[n++] = int( c.a()*255 );
        }

        RobotBase::myControlStep( this );

        RobotBase::mtx_enki.unlock();

        EPuck::controlStep( dt );
    }
}
