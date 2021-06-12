#include <chrono>

#ifdef WIN32
#define MSG_NOSIGNAL    0
#endif

#include "RobotBase.h"

RobotBase::RobotBase( std::string _name, std::string _tipo ) :
    running( false ), mtx(), mypos{ .0, .0 }, myspeed{ .0, .0 },
    mtx_enki(), name(_name), tipo( _tipo )
{
    std::cout << ">> Construyendo base para el robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
    //running.store( false );
}

RobotBase::~RobotBase()
{
    std::cout << ">> Destruyendo base del robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
}

void RobotBase::run( int sock )
{
    // no debe estar en ejecución
    if( !mtx.try_lock() )
    {
        std::cout << ">> Robot '" <<std::flush;
        std::cout << name << std::flush;
        std::cout << "' ya se encuentra en ejecucion" << std::endl;
        return;
    }

    // enviamos el tipo de robot que somos
    sendline( sock, tipo );

    // show time
    running.store( true );
    Json::Value json;
    Json::StreamWriterBuilder jwbuilder;
    jwbuilder["indentation"] = "";

    std::cout << ">> Robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "' ejecutando" << std::endl;
    while( running.load() )
    {
        // leemos la línea y verificamos la conexión
        int n = readline( sock );
        if( n == 0 ) continue;
        if( n < 0 ) break;

        // la línea debe venir en formato JSON
        try { std::stringstream( in_buffer ) >> json; }
        catch( ... ){ break; }

        // procesamos el comando
        std::string cmd = json.get( "cmd", "_UNDEF_" ).asString();
        Json::Value resp;
        //std::stringstream( "{}" ) >> resp;
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
            unsigned int len = 0;
            mtx_enki.lock();
            unsigned char* cameraImage = getCameraImage( &len );
            mtx_enki.unlock();

            if( cameraImage == nullptr ) break;
            sendbytes( sock, cameraImage, len );
            delete cameraImage;
            continue;
        }
        else
        {
            break;
        }

        // enviamos la respuesta
        sendline( sock, Json::writeString( jwbuilder, resp ) );
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

int RobotBase::readline( int sock )
{
    char c;
    unsigned int i = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while( i < sizeof( in_buffer )/sizeof( in_buffer[0] ) )
    {
        // si no ha llegado nada en los ultimos X segundos retornamos
        auto end = std::chrono::high_resolution_clock::now();
        if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= 1 ) return -i;

        int n = recv( sock, &c, 1, 0 );
        if( n == 1 )
        {
            if( c == '\n' )
            {
                if( i == 0 ) break;

                in_buffer[i]='\0';
                return i;
            }
            in_buffer[i++] = c;
            start = std::chrono::high_resolution_clock::now();
            continue;
        }
        if( n == 0 ) break;

        #ifdef WIN32
        if( WSAGetLastError() ==  WSAETIMEDOUT ) continue;
        #else
        if( errno == EAGAIN ) continue;
        #endif

        break;
    }
    return -1;
}

bool RobotBase::sendline( int sock, std::string text )
{
    std::string t = text + "\n";
    const char* buff = t.c_str();
    int i = 0;
    int l = t.length();
    while( i < l )
    {
        int n = send( sock, buff + i, 1, MSG_NOSIGNAL );
        if( n <= 0) return false;
        i += n;
    }
    return true;
}

bool RobotBase::sendbytes( int sock, unsigned char* data, unsigned int len )
{
    // los primeros 4 bytes serán el largo (BIG ENDIAN) de la data
    //char buff[ 4 + len ];
    char buff[512*3];
    buff[0] = 0;
    buff[1] = ( len & 0x00FF0000 ) >> 16;
    buff[2] = ( len & 0x0000FF00 ) >> 8;
    buff[3] = ( len & 0x000000FF );
    if( len != 0 ) memcpy ( buff+4, data, len );

    // enviamos la data
    unsigned int i = 0;
    len = 4 + len;
    while( i < len )
    {
        int n = send( sock, buff + i, 1, MSG_NOSIGNAL );
        if( n <= 0) return false;
        i += n;
    }
    return true;
}
