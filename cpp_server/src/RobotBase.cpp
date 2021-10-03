#include "RobotBase.hpp"
#include "Connection.hpp"

#include <chrono>
#include <math.h>

namespace RobWorld
{
    RobotBase::RobotBase( std::string _name, std::string _tipo ) :
        running( false ), mtx(), mypos{ .0, .0 }, myspeed{ .0, .0 },
        mtx_enki(), name(_name), tipo( _tipo ), myAngleRad(0)
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

        std::string buff;
        running.store( true );
        Json::Value json;
        Json::Value resp;
        Json::StreamWriterBuilder jwbuilder;
        jwbuilder["indentation"] = "";
        jwbuilder.settings_["precisionType"] = "decimal";
        jwbuilder.settings_["precision"] = 6;

        // enviamos el tipo de robot que somos
        std::stringstream( "{ \"type\":\"" + tipo + "\"}" ) >> resp;
        if( conn.sendData( Json::writeString( jwbuilder, resp ) ) )
        {
            // show time
            std::cout << ">> Robot '" << std::flush;
            std::cout << name << std::flush;
            std::cout << "' ejecutando" << std::endl;
            while( running.load() )
            {
                // leemos la línea y verificamos la conexión
                int n = conn.readData( buff, 1 );
                if( n == 0 ) continue;
                if( n < 0 ) break;

                json.clear();
                resp.clear();

                // la línea debe venir en formato JSON
                try { std::stringstream( buff ) >> json; }
                catch( ... ){ break; }

                // procesamos el comando
                std::string cmd;
                try { cmd = json.get( "cmd", "_UNDEF_" ).asString(); }
                catch( ... ){ break; }

                if( cmd.compare( "getSensors") == 0 )
                {
                    mtx_enki.lock();

                    resp["pos"] = Json::arrayValue;
                    for( unsigned int i = 0; i<sizeof(mypos)/sizeof(mypos[0]); i++ ) resp["pos"].append( mypos[i] );

                    resp["speed"] = Json::arrayValue;
                    for( unsigned int i = 0; i<sizeof(myspeed)/sizeof(myspeed[0]); i++ ) resp["speed"].append( myspeed[i] );

                    double grad = myAngleRad*(180/M_PI);
                    if( grad<0 ) grad = 360 + grad;
                    resp["angle"] = grad;

                    getSensors( resp );
                    mtx_enki.unlock();
                }
                else if( cmd.compare( "setSpeed") == 0 )
                {
                    double ls, rs;
                    try
                    {
                        ls = json.get( "leftSpeed", .0 ).asDouble();
                        rs = json.get( "rightSpeed", .0 ).asDouble();
                    }
                    catch( ... ){ break; }
                    mtx_enki.lock();
                    myspeed[0] = ls;
                    myspeed[1] = rs;
                    mtx_enki.unlock();
                }
                else if( cmd.compare( "setLedRing") == 0 )
                {
                    double st;
                    try
                    {
                        st = json.get( "estado", .0 ).asDouble();
                    }
                    catch( ... ){ break; }
                    mtx_enki.lock();
                    setLeds( &st, 1 );
                    mtx_enki.unlock();
                }
                else if( cmd.compare( "setLedsIntensity") == 0 )
                {
                    std::vector<double> leds;
                    try
                    {
                        Json::Value jleds = json["leds"];
                        unsigned int n = jleds.size();
                        for( unsigned int i=0; i<n; i++ ) leds.push_back( jleds[i].asDouble() );
                    }
                    catch( ... ){ break; }

                    if( leds.size() > 0 )
                    {
                        mtx_enki.lock();
                        setLeds( &leds[0], leds.size() );
                        mtx_enki.unlock();
                    }
                }
                else if( cmd.compare( "getCameraImage") == 0 )
                {
                    mtx_enki.lock();
                    BinaryData* cameraImage = getCameraImage();
                    mtx_enki.unlock();

                    bool ok = conn.sendData( cameraImage );
                    delete cameraImage;
                    if( !ok ) break;
                    continue;
                }
                else break;

                // enviamos la respuesta
                if( !conn.sendData( Json::writeString( jwbuilder, resp ) ) ) break;
            }
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
