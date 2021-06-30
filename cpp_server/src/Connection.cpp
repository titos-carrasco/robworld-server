#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>

    #ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
    #endif
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include "LineProtocol.hpp"
#include "GetProtocol.hpp"
#include "Connection.hpp"

#include <chrono>

#define MAXLINELENGTH   1024

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
        char c = 0;
        auto start = std::chrono::high_resolution_clock::now();
        while( true )
        {
            // si no ha llegado nada en los ultimos 'timeout' segundos marcamos error
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout ) break;

            int n = recv( sock, &c, 1, MSG_PEEK );
            if( n == 1 ) break;

            // socket cerrado
            if( n == 0 ) break;

            // error o timeout
            #ifdef WIN32
            if( WSAGetLastError() ==  WSAETIMEDOUT ) continue;
            #else
            if( errno == EAGAIN ) continue;
            #endif
            break;
        }
        if( c == 0 ) return nullptr;

        // Line Protocol
        if( c == '{' )
            return new LineProtocol( sock );

        // necesitamos la primera linea
        std::string line;
        int n = Connection::readline( sock, line, 1 );
        if( n <= 0 ) return nullptr;

        // --- HTTP --- WebSocket ---
        // GET /... HTTP/1.1\r"
        if( line.length() >= 15 && line.substr( line.length() - 10 ).compare( " HTTP/1.1\r" ) == 0 )
        {
            std::string method = "UNK";
            if( line.substr( 0, 5 ).compare( "GET /" ) == 0 ) method = "GET";
            //else if( line.substr( 0, 6 ).compare( "POST /" ) == 0 ) method = "POST";
            if( method.compare( "UNK" ) != 0 )
            {
                std::string path = line.substr( method.length() + 1, line.length() - method.length() - 1 - 10 );
                std::vector<std::string> headers;
                bool isWebsocket = false;
                while( true )
                {
                    int n = Connection::readline( sock, line, 1 );
                    if( n <= 0 ) return nullptr;

                    if( line.compare( "\r" ) == 0 ) break;
                    if( line.compare( "Upgrade: websocket\r" ) == 0 ) isWebsocket = true;
                    headers.push_back( line );
                }
                if( isWebsocket )
                    return nullptr;
                else if( method.compare( "GET" ) == 0 )
                    return new GetProtocol( sock, method, path, headers );
            }
        }

        // no soportado
        return nullptr;
    }

    int Connection::readline( int sock, std::string& data, unsigned int timeout )
    {
        unsigned char buff[MAXLINELENGTH+1];
        unsigned char c;
        unsigned int i = 0;
        auto start = std::chrono::high_resolution_clock::now();
        while( i < MAXLINELENGTH )
        {
            // si no ha llegado nada en los ultimos 'timeout' segundos retornamos
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout )
                return -i;

            int n = Connection::readbyte( sock, &c );
            if( n < 0 ) break;
            if( n == 0 ) continue;
            if( c == '\n' )
            {
                if( i == 0 ) break;

                buff[i]='\0';
                data = std::string( (char *)buff );
                return i;
            }
            buff[i++] = c;
            start = std::chrono::high_resolution_clock::now();
        }
        return -1;
    }

    int Connection::readbyte( int sock, unsigned char* c )
    {
        #ifdef WIN32
        int n = recv( sock, (char *)c, 1, 0 );
        #else
        int n = recv( sock, c, 1, 0 );
        #endif
        if( n == 1 ) return 1;
        if( n == 0 ) return -1;
        #ifdef WIN32
        if( WSAGetLastError() ==  WSAETIMEDOUT ) return 0;
        #else
        if( errno == EAGAIN ) return 0;
        #endif
        return -1;
    }

    bool Connection::sendline( int sock, std::string data )
    {
        std::string linea( data + "\n" );

        return sendbytes( sock, (const unsigned char* )linea.c_str(), linea.length() );
    }

    bool Connection::sendbinarydata( int sock, BinaryData* data )
    {
        unsigned int len = data->length();

        // los primeros 4 bytes serán el largo (BIG ENDIAN) de la data
        unsigned char buff[4];
        buff[0] = 0;
        buff[1] = ( len & 0x00FF0000 ) >> 16;
        buff[2] = ( len & 0x0000FF00 ) >> 8;
        buff[3] = ( len & 0x000000FF );

        // enviamos la data
        if( sendbytes( sock, buff, 4 ) )
            if( sendbytes( sock, data->getData(), len ) )
                return true;
        return false;
    }

    bool Connection::sendbytes( int sock, const unsigned char buff[], unsigned int len )
    {
        unsigned int i = 0;
        while( i < len )
        {
            #ifdef WIN32
            int n = send( sock, (char *)buff+i, len-i, MSG_NOSIGNAL );
            #else
            int n = send( sock, buff+i, len-i, MSG_NOSIGNAL );
            #endif
            if( n <= 0) return false;
            i += n;
        }
        return true;
    }
}
