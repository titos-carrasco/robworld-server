#ifdef WIN32
    #define R_SHUT_RDWR         SD_BOTH
#else
    #define R_SHUT_RDWR         SHUT_RDWR
    #define closesocket( s )    close( s )
#endif

#include "RobotWorld.hpp"
#include "Connection.hpp"
#include "RobotThymio2.hpp"
#include "RobotEPuck.hpp"
#include "RobotMarxbot.hpp"

#include <fstream>
#include <viewer/Viewer.h>

namespace RobWorld
{
    RobotWorld::RobotWorld( std::string world_file_name ) :
        world( nullptr ), walls( 10 )
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
        Json::Value json;

        std::ifstream fs( world_file_name );
        fs >> json;
        fs.close();

        // procesamos cada definición del mundo
        std::cout << ">> Creando el mundo" << std::endl;

        std::map<std::string, Enki::Color> colors;
        for( auto& entry : json )
        {
            // necesitamos el tipo de elemento a crear
            std::string type = entry.get( "type", "_UNDEF_" ).asString();
            if( type.compare( "_UNDEF_" ) == 0 )
            {
                std::cout << ">> No especifica 'type': " << std::endl;
                std::cout << entry << std::endl;
                continue;
            }

            // colores
            if( type.compare( "color" ) == 0 )
            {
                std::string name = entry.get( "name", "_UNDEF_" ).asString();
                double r = entry.get( "r", -1 ).asInt();
                double g = entry.get( "g", -1 ).asInt();
                double b = entry.get( "b", -1 ).asInt();
                double a = entry.get( "a", -1 ).asInt();

                if( name.compare( "_UNDEF_" ) == 0 ||
                    ( r < 0 || r > 255 ) ||
                    ( g < 0 || g > 255 ) ||
                    ( b < 0 || b > 255 ) ||
                    ( a < 0 || a > 255 ) )
                {
                    std::cout << ">> Definicion de 'color' invalida: " << std::endl;
                    std::cout << entry << std::endl;
                }
                else
                {

                    colors[ name ] = Enki::Color( r/255., g/255., b/255, a/255. );
                }
                continue;
            }

            // el mundo
            if( type.compare( "world" ) == 0 )
            {
                if( world != nullptr )
                {
                    std::cout << ">> Definicion de 'world' ya existe: " << std::endl;
                    std::cout << entry << std::endl;
                    continue;
                }

                double width = entry.get( "width", -1 ).asDouble();
                double height = entry.get( "height", -1 ).asDouble();
                walls = entry.get( "walls", -1 ).asDouble();
                std::string color = entry.get( "color", "_UNDEF_" ).asString();
                std::string ground = entry.get( "ground", "_UNDEF_" ).asString();
                std::string host = entry.get( "host", "_UNDEF_" ).asString();
                int port = entry.get( "port", -1 ).asInt() ;

                if( width <= 0 ||
                    height <= 0 ||
                    walls < 0 ||
                    color.compare( "_UNDEF_" ) == 0 ||
                    ground.compare( "_UNDEF_" ) == 0 ||
                    host.compare( "_UNDEF_" ) == 0 ||
                    port <= 0 )
                {
                    std::cout << entry << std::endl;
                    throw std::runtime_error( "Definicion del mundo es invalida" );
                }


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

                    QString f = QString::fromLocal8Bit( fname.c_str() );
                    QImage gnd( f );
                    if( gnd.format() > 0 )
                    {
                        QImage mirror = gnd.mirrored( false, true ).convertToFormat( QImage::Format_ARGB32 );
                        gt = Enki::World::GroundTexture( mirror.width(), mirror.height(), (const uint32_t*)mirror.constBits() );
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
                if( world == nullptr )
                    throw std::runtime_error( "Definicion del mundo no encontrada" );

                double x = entry.get( "x", -1 ).asDouble();
                double y = entry.get( "y", -1 ).asDouble();
                double sizex = entry.get( "sizex", -1 ).asDouble();
                double sizey = entry.get( "sizey", -1 ).asDouble();
                double height = entry.get( "height", -1 ).asDouble();
                double mass = entry.get( "mass", -2 ).asDouble();
                std::string color = entry.get( "color", "_UNDEF_" ).asString();

                if( x < 0 ||
                    y < 0 ||
                    sizex <= 0 ||
                    sizey <= 0 ||
                    height <= 0 ||
                    mass <= -2 ||
                    color.compare( "_UNDEF_" ) == 0 )
                {
                    std::cout << ">> Definicion de 'box' invalida: " << std::endl;
                    std::cout << entry << std::endl;
                }
                else
                {

                    Enki::PhysicalObject* o = new Enki::PhysicalObject();
                    o->setRectangular( sizex, sizey, height, mass );
                    o->setColor( colors[ color ]  );
                    o->pos = Enki::Point( x + sizex/2, y + sizey/2 );
                    o->angle = entry.get( "angle", .0 ).asDouble()*(M_PI/180.0);
                    world->addObject( o );
                }
                continue;
            }

            // cilindros
            if( type.compare( "cylinder" ) == 0 )
            {
                if( world == nullptr )
                    throw std::runtime_error( "Definicion del mundo no encontrada" );

                double x = entry.get( "x", -1 ).asDouble();
                double y = entry.get( "y", -1 ).asDouble();
                double radius = entry.get( "radius", -1 ).asDouble();
                double height = entry.get( "height", -1 ).asDouble();
                double mass = entry.get( "mass", -2 ).asDouble();
                std::string color = entry.get( "color", "_UNDEF_" ).asString();

               if( x < 0 ||
                   y < 0 ||
                   radius <= 0 ||
                   height <= 0 ||
                   mass <= -2 ||
                   color.compare( "_UNDEF_" ) == 0 )
                {
                    std::cout << ">> Definicion de 'cylinder' invalida: " << std::endl;
                    std::cout << entry << std::endl;
                }
                else
                {
                    Enki::PhysicalObject* o = new Enki::PhysicalObject();
                    o->setCylindric( radius, height, mass );
                    o->setColor( colors[ color ]  );
                    o->pos = Enki::Point( x, y );
                    o->angle = entry.get( "angle", .0 ).asDouble()*(M_PI/180.0);
                    world->addObject( o );
                }
                continue;
            }


            // robot EPuck(Thymio2/MarxBot
            if( type.compare( "epuck" ) == 0 || type.compare( "thymio2" ) || type.compare( "marxbot" ))
            {
                if( world == nullptr )
                    throw std::runtime_error( "Definicion del mundo no encontrada al procesar robot" );

                std:: string name = entry.get( "name", "_UNDEF_" ).asString();
                double x = entry.get( "x", -1 ).asDouble();
                double y = entry.get( "y", -1 ) .asDouble();

                if( name.compare( "_UNDEF_" )  == 0 ||
                    x < 0 ||
                    y < 0 )
                {
                    std::cout << ">> Definicion de '" << std::flush;
                    std::cout << type << std::flush;
                    std::cout << ">> Definicion de 'EPuck' invalida: " << std::endl;
                    std::cout << entry << std::endl;
                }
                else
                {

                    try{
                        robots.at( name );
                        std::cout << ">> Robot con este nombre ya existe: " << std::endl;
                        std::cout << entry << std::endl;
                    }
                    catch( const std::out_of_range& err )
                    {
                        //RobotBase* r = nullptr;
                        if( type.compare( "epuck" ) == 0 )
                        {
                            RobotEPuck* r = new RobotEPuck( name, Enki::EPuck::CAPABILITY_BASIC_SENSORS | Enki::EPuck::CAPABILITY_CAMERA );
                            r->pos = Enki::Point( x, y );
                            r->angle = entry.get( "angle", .0 ).asDouble()*(M_PI/180.0);
                            robots[ name ] = r;
                            world->addObject( r );
                        }
                        else if( type.compare( "thymio2" ) == 0 )
                        {
                            RobotThymio2* r = new RobotThymio2( name );
                            r->pos = Enki::Point( x, y );
                            r->angle = entry.get( "angle", .0 ).asDouble()*(M_PI/180.0);
                            robots[ name ] = r;
                            world->addObject( r );
                        }
                        else if( type.compare( "marxbot" ) == 0 )
                        {
                            RobotMarxbot* r = new RobotMarxbot( name );
                            r->pos = Enki::Point( x, y );
                            r->angle = entry.get( "angle", .0 ).asDouble()*(M_PI/180.0);
                            robots[ name ] = r;
                            world->addObject( r );
                        }

                    }
                }
                continue;
            }

            std::cout << ">> Linea no reconocida: " << std::endl;
            std::cout << entry << std::endl;
        }

        if( world == nullptr )
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
        delete tDispatcher;

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
            // revisamos cada 1 segundo si hay actividad en el socket
            FD_ZERO( &readfds );
            FD_SET( srv_sock, &readfds );
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            if( ::select( srv_sock + 1 , &readfds , nullptr , nullptr , &timeout ) < 0 ) continue;
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
        // socket timeout
        struct ::timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        #ifdef WIN32
        ::setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof( struct timeval ) );
        #else
        ::setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( struct timeval ) );
        #endif

        Connection* conn = Connection::getConnector( sock, 3 );
        if( conn != nullptr )
        {
            Json::Value json;
            std::string buff;
            int n = conn->readData( buff, 1 );

            if( n == 0 ) std::cout << ">> Timeout" << std::endl;
            else if( n < 0 ) std::cout << ">> Error en la conexion" << std::endl;
            else
            {
                // la línea de conexión debe venir en formato JSON
                try
                {
                    std::stringstream( buff ) >> json;
                }
                catch( ... )
                {
                    std::cout << ">> Comando de conexion es invalido" << std::endl;
                    goto _ABORT;
                }

                // recuperamos el nombre del robot con el que se desea trabajar
                std::string name = json.get( "connect", "_UNDEF_" ).asString();
                if( name.compare( "_UNDEF_") == 0 )
                    std::cout << ">> Comando de conexion es invalido" << std::endl;
                else
                {
                    // el robot debe existir
                    RobotBase*  r;
                    try{
                        r = robots.at( name );
                    }
                    catch( ... )
                    {
                        std::cout << ">> Robot solicitado no existe" << std::endl;
                        goto _ABORT;
                    }

                    // el robot toma el control
                    r->run( *conn );
                }
            }
_ABORT:     delete conn;
        }
        else
            std::cout << ">> Protocolo no reconocido" << std::endl;

        // fin del hilo
        try { ::shutdown( sock, R_SHUT_RDWR ); }
        catch( ... ) {}
        closesocket( sock );
    }
}
