#include <chrono>

#include "RobotBase.h"

RobotBase::RobotBase( std::string _name, std::string _tipo ) :
    running( false ), mtx(), mypos{ .0, .0 }, myspeed{ .0, .0 },
    mtx_enki(), name(_name), tipo( _tipo )
{
    std::cout << ">> Construyendo base para el robot '";
    std::cout << name << std::flush;
    std::cout << "'" << std::endl;
    //running.store( false );
}

RobotBase::~RobotBase()
{
    std::cout << ">> Destruyendo base del robot '";
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
    std::cout << ">> Robot '" << std::flush;
    std::cout << name << std::flush;
    std::cout << "' ejecutando" << std::endl;
    while( running.load() )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

        // leemos la línea y verificamos la conexión
        std::string* linea;
        int n = readline( sock, &linea );
        if( n == 0 ) continue;
        if( n < 0 ) break;

        // la línea debe venir en formato JSON
        JSON json = JSON::parse( *linea, nullptr, false, false );
        delete linea;
        if( json.is_discarded() ) break;

        // obtenemos el nombre del comando
        std::string cmd;
        try{ cmd = json["cmd"]; }
        catch( ... ){ break; }

        // procesamos el comando
        JSON resp = "{}"_json;
        if( cmd.compare( "getSensors") == 0 )
        {
            resp["pos"] = mypos;
            resp["speed"] = myspeed;
            getSensors( resp );
        }
        else if( cmd.compare( "setSpeed") == 0 )
        {
            try
            {
                double ls = json["leftSpeed"];
                double rs = json["rightSpeed"];
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
                double st = json["estado"];
                setLeds( &st, 1 );
            }
            catch( ... ){ break; }
        }
        else if( cmd.compare( "setLedsIntensity") == 0 )
        {
            try
            {
                std::vector<double> leds = json["leds"];
                unsigned int n = leds.size();
                setLeds( &leds[0], n );
            }
            catch( ... ){ break; }
        }
        else if( cmd.compare( "getCameraImage") == 0 )
        {
            unsigned int len = 0;
            unsigned char* cameraImage = getCameraImage( &len );
            sendbytes( sock, cameraImage, len );
            if( cameraImage != NULL ) delete cameraImage;
            continue;
        }
        else
        {
            break;
        }

        // enviamos la respuesta
        sendline( sock, resp.dump() );
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
    std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

    mtx_enki.lock();

    // hacia el robot
    o->leftSpeed = myspeed[0];
    o->rightSpeed = myspeed[1];

    // desde el robot
    mypos[0] = o->pos.x;
    mypos[1] = o->pos.y;

    mtx_enki.unlock();
}

int RobotBase::readline( int sock, std::string** linea )
{
    char buff[256];
    char c;
    unsigned int i = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while( i < sizeof( buff ) )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

        // si no ha llegado nada en los ultimos X segundos retornamos
        auto end = std::chrono::high_resolution_clock::now();
        if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= 1 ) return -i;

        #ifdef WIN32
        int n = recv( sock, &c, 1, 0 );
        #else
        int n = read( sock, &c, 1 );
        #endif
        if( n == 1 )
        {
            if( c == '\n' )
            {
                buff[i]='\0';
                if( i > 0 ) *linea = new std::string( buff );
                return i;
            }
            buff[i++] = c;
            start = std::chrono::high_resolution_clock::now();
            continue;
        }
        if( n == 0 ) break;
        #ifdef WIN32
        if( WSAGetLastError() == WSAEWOULDBLOCK ) continue;
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
        #ifdef WIN32
        int n = send( sock, buff + i, 1, 0 );
        #else
        int n = write( sock, buff + i, 1 );
        #endif
        if( n <= 0) return false;
        i += n;
    }
    return true;
}

bool RobotBase::sendbytes( int sock, unsigned char* data, unsigned int len )
{
    // los primeros 4 bytes serán el largo (BIG ENDIAN) de la data
    char buff[ 4 + len ];
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
        #ifdef WIN32
        int n = send( sock, buff + i, 1, 0 );
        #else
        int n = write( sock, buff + i, 1 );
        #endif
        if( n <= 0) return false;
        i += n;
    }
    return true;
}
