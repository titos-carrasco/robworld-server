#include "RobotMarxbot.hpp"

namespace RobWorld
{
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

    void RobotMarxbot::getSensors( Json::Value& resp )
    {
        resp["virtualBumpers"] = Json::arrayValue;
        for( unsigned int i = 0; i<sizeof(myVirtualBumpers)/sizeof(myVirtualBumpers[0]); i++ ) resp["virtualBumpers"].append( myVirtualBumpers[i] );
    }

    void RobotMarxbot::setLeds( const double leds[], int nleds )
    {
    }

    BinaryData* RobotMarxbot::getCameraImage()
    {
        return new BinaryData( 0, 0 );
    }

    void RobotMarxbot::controlStep( double dt )
    {
        RobotBase::mtx_enki.lock();

        // hacia el robot

        // desde el robot
        int n = int( sizeof( myVirtualBumpers )/sizeof( myVirtualBumpers[0] ) );
        for( int i=0; i < n; i++ )
            myVirtualBumpers[i] = Marxbot::getVirtualBumper( i );

        myAngleRad = angle;

        RobotBase::myControlStep( this );

        RobotBase::mtx_enki.unlock();

        Marxbot::controlStep( dt );
    }
}

