#ifdef WIN32
    #define R_SHUT_RDWR SD_BOTH
#else
    #define R_SHUT_RDWR SHUT_RDWR
    #define closesocket( s )    close( s )
#endif // WIN32

#include <thread>
#include <chrono>
#include <ctime>
#include <vector>
#include <fstream>
#include <fcntl.h>

#include <json.hpp>
#include "RobotWorld.h"

using JSON = nlohmann::json;

RobotWorld::RobotWorld( std::string world_file_name ) :
    world( NULL ), walls( 10 )
{
    // creamos el socket
    std::cout << ">> Configurando socket" << std::endl;

    #ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif
    if( ( srv_sock = ::socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        throw std::runtime_error( "No se pudo crear el socket" );

    // lo marcamos para poder recibir varias conexiones a la misma puerta
    #ifdef WIN32
    int opt = 1;
    if( ::setsockopt( srv_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof( opt ) ) < 0 )
        throw std::runtime_error( "No se pudo configurar el socket" );
    #else
    int opt = 1;
    if( ::setsockopt( srv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ) ) < 0 )
        throw std::runtime_error( "No se pudo configurar el socket" );
    #endif

    // configuramos sus datos de escucha
    ::memset( &srv_address, 0, sizeof( srv_address ) );
    srv_address.sin_family = AF_INET;

    // leemos el archivo del mundo que está en formato JSON
    std::cout << ">> Leyendo archivo 'world'" << std::endl;

    std::ifstream fs( world_file_name );
    JSON json;
    fs >> json;
    fs.close();

    // procesamos cada definición del mundo
    std::cout << ">> Creando el mundo" << std::endl;

    std::map<std::string, Enki::Color> colors;
    for( auto& elem : json.items() )
    {
        // necesitamos el tipo de elemento a crear
        auto& entry = elem.value();
        if( entry.find( "type" ) == entry.end() )
        {
            std::cout << ">> No especifica 'type': " << std::endl;
            std::cout << entry << std::endl;
            continue;
        }
        std::string type = entry["type"];

        // colores
        if( type.compare( "color" ) == 0 )
        {
            if( entry.find( "name" ) == entry.end() ||
                entry.find( "r" )    == entry.end() ||
                entry.find( "g" )    == entry.end() ||
                entry.find( "b" )    == entry.end() ||
                entry.find( "a" )    == entry.end() )
            {
                std::cout << ">> Definicion de 'color' invalida: " << std::endl;
                std::cout << entry << std::endl;
            }
            else
            {
                std::string name = entry["name"];
                double r = entry["r"];
                double g = entry["g"];
                double b = entry["b"];
                double a = entry["a"];

                colors[ name ] = Enki::Color( r/255., g/255., b/255, a/255. );
            }
            continue;
        }

        // el mundo
        if( type.compare( "world" ) == 0 )
        {
            if( world != NULL )
            {
                std::cout << ">> Definicion de 'world' ya existe: " << std::endl;
                std::cout << entry << std::endl;
                continue;
            }

            if( entry.find( "width" )  == entry.end() ||
                entry.find( "height" ) == entry.end() ||
                entry.find( "walls" )  == entry.end() ||
                entry.find( "color" )  == entry.end() ||
                entry.find( "ground" ) == entry.end() ||
                entry.find( "host" )   == entry.end() ||
                entry.find( "port" )   == entry.end() )
            {
                throw std::runtime_error( "Definicion del mundo es invalida" );
            }

            double width = entry["width"];
            double height = entry["height"];
            walls = entry["walls"];
            std::string ground = entry["ground"];
            std::string color = entry["color"];
            std::string host = entry["host"];
            int port = entry["port"];

            Enki::World::GroundTexture gt;
            if( ground.length() > 0 )
            {
                std::string fname( ground );
                auto pos = world_file_name.find_last_of( "/" );
                if( pos != std::string::npos )
                    fname = world_file_name.substr( 0, pos ) + "/" + ground;
                else
                {
                    pos = world_file_name.find_last_of( "\\" );
                    if( pos != std::string::npos )
                        fname = world_file_name.substr( 0, pos ) + "\\" + ground;
                }

                QImage xpm( fname.c_str() );
                if( xpm.format() > 0 )
                {
                    QImage qgt( QGLWidget::convertToGLFormat( xpm ) );
                    gt = Enki::World::GroundTexture( qgt.width(), qgt.height(), (const uint32_t*)qgt.constBits() );
                }
            }
            world = new Enki::World( width, height, Enki::Color( colors[color] ), gt );

            // host y port para el socket
            if( host.length() == 0 ) host = "0.0.0.0";
            srv_address.sin_addr.s_addr = inet_addr( host.c_str() );
            srv_address.sin_port = htons( port );

            continue;
        }

        // cajas
        if( type.compare( "box" ) == 0 )
        {
            if( world == NULL )
                throw std::runtime_error( "Definicion del mundo no encontrada" );

            if( entry.find( "x" )      == entry.end() ||
                entry.find( "y" )      == entry.end() ||
                entry.find( "l1" )     == entry.end() ||
                entry.find( "l2" )     == entry.end() ||
                entry.find( "height" ) == entry.end() ||
                entry.find( "mass" )   == entry.end() ||
                entry.find( "color" )  == entry.end() )
            {
                std::cout << ">> Definicion de 'box' invalida: " << std::endl;
                std::cout << entry << std::endl;
            }
            else
            {
                double x = entry["x"];
                double y = entry["y"];
                double l1 = entry["l1"];
                double l2 = entry["l2"];
                double height = entry["height"];
                double mass = entry["mass"];
                std::string color = entry["color"];

                Enki::PhysicalObject* o = new Enki::PhysicalObject();
                o->setRectangular( l1, l2, height, mass );
                o->setColor( colors[ color ]  );
                o->pos = Enki::Point( x, y );
                o->angle = entry.find( "angle" ) == entry.end() ? .0 : (double)entry["angle"]*(M_PI/180.0);
                world->addObject( o );
            }
            continue;
        }

        // cilindros
        if( type.compare( "cylinder" ) == 0 )
        {
            if( world == NULL )
                throw std::runtime_error( "Definicion del mundo no encontrada" );

            if( entry.find( "x" )      == entry.end() ||
                entry.find( "y" )      == entry.end() ||
                entry.find( "radius" ) == entry.end() ||
                entry.find( "height" ) == entry.end() ||
                entry.find( "mass" )   == entry.end() ||
                entry.find( "color" )  == entry.end() )
            {
                std::cout << ">> Definicion de 'cylinder' invalida: " << std::endl;
                std::cout << entry << std::endl;
            }
            else
            {
                double x = entry["x"];
                double y = entry["y"];
                double radius = entry["radius"];
                double height = entry["height"];
                double mass = entry["mass"];
                std::string color = entry["color"];

                Enki::PhysicalObject* o = new Enki::PhysicalObject();
                o->setCylindric( radius, height, mass );
                o->setColor( colors[ color ]  );
                o->pos = Enki::Point( x, y );
                o->angle = entry.find( "angle" ) == entry.end() ? .0 : (double)entry["angle"]*(M_PI/180.0);
                world->addObject( o );
            }
            continue;
        }

        // robot EPuck
        if( type.compare( "epuck" ) == 0 )
        {
            if( world == NULL )
                throw std::runtime_error( "Definicion del mundo no encontrada" );

            if( entry.find( "name" )  == entry.end() ||
                entry.find( "x" )     == entry.end() ||
                entry.find( "y" )     == entry.end() )
            {
                std::cout << ">> Definicion de 'EPuck' invalida: " << std::endl;
                std::cout << entry << std::endl;
            }
            else
            {
                std:: string name = entry["name"];
                double x = entry["x"];
                double y = entry["y"];

                try{
                    robots.at( name );
                    std::cout << ">> Robot con este nombre ya existe: " << std::endl;
                    std::cout << entry << std::endl;
                }
                catch( const std::out_of_range& err )
                {
                    RobotEPuck *r = new RobotEPuck( name, Enki::EPuck::CAPABILITY_BASIC_SENSORS | Enki::EPuck::CAPABILITY_CAMERA );
                    r->pos = Enki::Point( x, y );
                    r->angle = entry.find( "angle" ) == entry.end() ? .0 : (double)entry["angle"]*(M_PI/180.0);
                    world->addObject( r );
                    robots[ name ] = r;
                }
            }
            continue;
        }

        // robot Thymio2
        if( type.compare( "thymio2" ) == 0 )
        {
            if( world == NULL )
                throw std::runtime_error( "Definicion del mundo no encontrada" );

            if( entry.find( "name" )  == entry.end() ||
                entry.find( "x" )     == entry.end() ||
                entry.find( "y" )     == entry.end() )
            {
                std::cout << ">> Definicion de 'Thymio2' invalida: " << std::endl;
                std::cout << entry << std::endl;
            }
            else
            {
                std:: string name = entry["name"];
                double x = entry["x"];
                double y = entry["y"];

                try{
                    robots.at( name );
                    std::cout << ">> Robot con este nombre ya existe: " << std::endl;
                    std::cout << entry << std::endl;
                }
                catch( const std::out_of_range& err )
                {
                    RobotThymio2 *r = new RobotThymio2( name );
                    r->pos = Enki::Point( x, y );
                    r->angle = entry.find( "angle" ) == entry.end() ? .0 : (double)entry["angle"]*(M_PI/180.0);
                    world->addObject( r );
                    robots[ name ] = r;
                }
            }
            continue;
        }

        std::cout << ">> Linea no reconocida: " << std::endl;
        std::cout << entry << std::endl;
    }

    if( world == NULL )
        throw std::runtime_error( "Definicion del mundo no encontrada" );

    // hacemos el bind aqui ya que en el archivo world vienen el host y la port
    // enlazamos el socket con sus parametros
    if( ::bind( srv_sock, (struct ::sockaddr *)&srv_address, sizeof( srv_address) ) < 0 )
        throw std::runtime_error( "No se pudo realizar el bind del socket" );

    // lo ponemos en modo pasivo
    if( ::listen( srv_sock, 3 ) < 0 )
        throw std::runtime_error( "No se pudo configurar el socket en modo pasivo" );

    std::cout << ">> Mundo creado" << std::endl;
}

RobotWorld::~RobotWorld()
{
    std::cout << ">> Destruyendo el mundo de robots" << std::endl;
    delete world;
}

Enki::World* RobotWorld::getWorld()
{
    return world;
}

double RobotWorld::getWalls()
{
    return walls;
}

// el manejador de conexiones lo trabajamos en un hilo
void RobotWorld::run()
{
    std::cout << ">> Iniciando despachador de conexiones" << std::endl;
    tDispatcherRunning.store( false );
    tDispatcher = new std::thread( &RobotWorld::dispatcher, this );
    while( !tDispatcherRunning.load() )
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
}

// fin del juego
void RobotWorld::stop()
{
    std::cout << ">> Finalizando despachador de conexiones" << std::endl;
    tDispatcherRunning.store( false );
    tDispatcher->join();

    #ifdef WIN32
    WSACleanup();
    #endif
}

// el manejador de conexiones
void RobotWorld::dispatcher()
{
    std::cout << ">> Esperando conexiones en " << std::flush;
    std::cout << inet_ntoa( srv_address.sin_addr ) << std::flush;
    std::cout << ":" << std::flush;
    std::cout << ntohs( srv_address.sin_port ) << std::flush;
    std::cout << std::endl;

    ::fd_set readfds;
    struct ::sockaddr_in address;
    int addrlen = sizeof( address );
    struct ::timeval timeout;

    std::vector<std::thread> threads;
    tDispatcherRunning.store( true );
    while( tDispatcherRunning.load() )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

        // revisamos cada 1 segundo si hay actividad en el socket
        FD_ZERO( &readfds );
        FD_SET( srv_sock, &readfds );
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        if( ::select( srv_sock + 1 , &readfds , NULL , NULL , &timeout ) < 0 ) continue;
        if( !FD_ISSET( srv_sock, &readfds ) ) continue;

        // conexion recibida
        int client = ::accept( srv_sock, (struct ::sockaddr *)&address, (socklen_t*)&addrlen );
        std::cout << ">> Conexion recibida desde " << std::flush;
        std::cout << inet_ntoa( address.sin_addr ) << std::flush;
        std::cout << ":" << std::flush;
        std::cout << ntohs( address.sin_port ) << std::flush;
        std::cout << std::endl;

        // procesamos esta conexion en otro hilo
        threads.push_back( std::thread( &RobotWorld::TRobot, this, client ) );

        // quizas eliminar del vector de threads aquellos que ya no estan en ejecucion
    }

    std::cout << ">> Cerrando el socket" << std::endl;
    try { shutdown( srv_sock, R_SHUT_RDWR ); }
    catch( ... ) {}
    closesocket( srv_sock );

    std::cout << ">> Deteniendo robots" << std::endl;
    for (auto const& elem : robots)
        (elem.second)->stop();

    std::cout << ">> Finalizando threads" << std::endl;
    for (auto& th : threads)
        th.join();
}

// este es el hilo de un robot
void RobotWorld::TRobot( int sock )
{
    // non blocking socket
    #ifdef WIN32
        u_long mode = 1;
        ioctlsocket(sock, FIONBIO, &mode );
    #else
        ::fcntl( sock, F_SETFL, fcntl( sock, F_GETFL, 0) | O_NONBLOCK );
    #endif // WIN32

    char buff[128];
    char c;
    unsigned int i = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while( i < sizeof( buff ) )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

        // 4 segundos para recibir el nombre del robot a controlar
        auto end = std::chrono::high_resolution_clock::now();
        if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= 4 )
        {
            std::cout << ">> Timeout" << std::endl;
            break;
        }

        #ifdef WIN32
        int n = recv( sock, &c, 1, 0  );
        #else
        int n = read( sock, &c, 1 );
        #endif
        if( n == 1 )
        {
            if( c == '\n' )
            {
                buff[i]='\0';

                // si existe toma el control de este hilo
                try{
                    std::string name( buff );
                    RobotBase* r = robots.at( name );
                    r->run( sock );
                }
                catch( const std::out_of_range& err )
                {
                    std::cout << ">> Robot solicitado no existe" << std::endl;
                }
                break;
            }
            buff[i++] = c;
            continue;
        }

        if( n == 0 )
        {
            std::cout << ">> Conexion abortada" << std::endl;
            break;
        }

        #ifdef WIN32
        if( WSAGetLastError() == WSAEWOULDBLOCK ) continue;
        #else
        if( errno == EAGAIN ) continue;
        #endif

        std::cout << ">> Error en la conexion" << std::endl;
        break;
    }
    if( i >= sizeof( buff ) )
        std::cout << ">> Nombre de robot es invadlido" << std::endl;

    // fin del hilo
    try { ::shutdown( sock, R_SHUT_RDWR ); }
    catch( ... ) {}
    closesocket( sock );
}
