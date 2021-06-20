#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif
#include <chrono>
#include "LineProtocol.hpp"
#include "Connection.hpp"

namespace RobWorld
{
    Connection::Connection()
    {
    }

    Connection::~Connection()
    {
    }

    Connection* Connection::getConnector( int sock, unsigned int timeout )
    {
        char c;
        auto start = std::chrono::high_resolution_clock::now();
        while( true )
        {
            // si no ha llegado nada en los ultimos 'timeout' segundos marcamos error
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout ) break;

            int n = recv( sock, &c, 1, MSG_PEEK );

            // socket cerrado
            if( n == 0 ) break;

            // error o timeout
            if( n < 0 )
            {
                #ifdef WIN32
                if( WSAGetLastError() ==  WSAETIMEDOUT ) continue;
                #else
                if( errno == EAGAIN ) continue;
                #endif
            }

            // Line Protocol
            if( c == '{' )
            {
                return new LineProtocol( sock );
            }

            break;
        }
        return nullptr;
    }
}
