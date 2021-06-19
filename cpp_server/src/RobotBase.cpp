#ifdef WIN32
#define MSG_NOSIGNAL    0
#endif

#include <chrono>

#include "RobotBase.hpp"
#include "Connection.hpp"

namespace RobWorld
{
    RobotBase::RobotBase( std::string _name, std::string _tipo ) :
        running( false ), mtx(), mypos{ .0, .0 }, myspeed{ .0, .0 },
        mtx_enki(), name(_name), tipo( _tipo )
    {
        std::cout << ">> Construyendo base para el robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "'" << std::endl;
    }

    RobotBase::~RobotBase()
    {
        std::cout << ">> Destruyendo base del robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "'" << std::endl;
    }

    void RobotBase::run( Connection& conn )
    {
        // no debe estar en ejecución
        if( !mtx.try_lock() )
        {
            std::cout << ">> Robot '" <<std::flush;
            std::cout << name << std::flush;
            std::cout << "' ya se encuentra en ejecucion" << std::endl;
            return;
        }

        running.store( true );
        Json::Value json;
        Json::Value resp;
        Json::StreamWriterBuilder jwbuilder;
        jwbuilder["indentation"] = "";

        // enviamos el tipo de robot que somos
        std::stringstream( "{ \"type\":\"" + tipo + "\"}" ) >> resp;
        conn.sendline( Json::writeString( jwbuilder, resp ) );

        // show time
        std::cout << ">> Robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "' ejecutando" << std::endl;
        char buff[512];
        while( running.load() )
        {
            // leemos la línea y verificamos la conexión
            int n = conn.readline( buff, sizeof( buff )/sizeof( buff[0] ), 1 );
            if( n == 0 ) continue;
            if( n < 0 ) break;

            json.clear();
            resp.clear();

            // la línea debe venir en formato JSON
            try { std::stringstream( buff ) >> json; }
            catch( ... ){ break; }

            // procesamos el comando
            std::string cmd = json.get( "cmd", "_UNDEF_" ).asString();
            if( cmd.compare( "getSensors") == 0 )
            {
                mtx_enki.lock();

                resp["pos"] = Json::arrayValue;
                for( unsigned int i = 0; i<sizeof(mypos)/sizeof(mypos[0]); i++ ) resp["pos"].append( mypos[i] );

                resp["speed"] = Json::arrayValue;
                for( unsigned int i = 0; i<sizeof(myspeed)/sizeof(myspeed[0]); i++ ) resp["speed"].append( myspeed[i] );

                getSensors( resp );
                mtx_enki.unlock();
            }
            else if( cmd.compare( "setSpeed") == 0 )
            {
                try
                {
                    double ls = json.get( "leftSpeed", .0 ).asDouble();
                    double rs = json.get( "rightSpeed", .0 ).asDouble();
                    mtx_enki.lock();
                    myspeed[0] = ls;
                    myspeed[1] = rs;
                    mtx_enki.unlock();
                }
                catch( ... ){ break; }
            }
            else if( cmd.compare( "setLedRing") == 0 )
            {
                try
                {
                    double st = json.get( "estado", .0 ).asDouble();
                    mtx_enki.lock();
                    setLeds( &st, 1 );
                    mtx_enki.unlock();
                }
                catch( ... ){ break; }
            }
            else if( cmd.compare( "setLedsIntensity") == 0 )
            {
                try
                {
                    Json::Value jleds = json["leds"];
                    unsigned int n = jleds.size();
                    std::vector<double> leds;
                    for( unsigned int i=0; i<n; i++ ) leds.push_back( jleds[i].asDouble() );

                    mtx_enki.lock();
                    setLeds( &leds[0], n );
                    mtx_enki.unlock();
                }
                catch( ... ){ break; }
            }
            else if( cmd.compare( "getCameraImage") == 0 )
            {
                mtx_enki.lock();
                #include "Connection.hpp"
                BinaryData* cameraImage = getCameraImage();
                mtx_enki.unlock();

                bool ok = conn.sendbinarydata( cameraImage );
                delete cameraImage;
                if( !ok ) break;
                continue;
            }
            else
            {
                break;
            }

            // enviamos la respuesta
            if( !conn.sendline( Json::writeString( jwbuilder, resp ) ) ) break;
        }

        // esto es todo
        std::cout << ">> Robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "' finalizado" << std::endl;
        mtx.unlock();
    }

    void RobotBase::stop()
    {
        std::cout << ">> Deteniendo robot '" << std::flush;
        std::cout << name << std::flush;
        std::cout << "'" << std::endl;
        running.store( false );
    }

    void RobotBase::myControlStep( Enki::DifferentialWheeled* o )
    {
        // hacia el robot
        o->leftSpeed = myspeed[0];
        o->rightSpeed = myspeed[1];

        // desde el robot
        mypos[0] = o->pos.x;
        mypos[1] = o->pos.y;

    }
}
