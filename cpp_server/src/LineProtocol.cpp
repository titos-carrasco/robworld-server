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
#include "BinaryData.hpp"
#include "LineProtocol.hpp"

namespace RobWorld
{
    LineProtocol::LineProtocol( int _sock ) :
        sock( _sock )
    {
    }

    LineProtocol::~LineProtocol()
    {
    }

    int LineProtocol::readData( std::string& data, unsigned int timeout )
    {
        unsigned char* buff = new unsigned char[MAXLINELENGTH];
        char c;
        unsigned int i = 0;
        auto start = std::chrono::high_resolution_clock::now();
        while( i < MAXLINELENGTH )
        {
            // si no ha llegado nada en los ultimos 'timeout' segundos retornamos
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout )
            {
                delete buff;
                return -i;
            }

            int n = recv( sock, &c, 1, 0 );
            if( n == 1 )
            {
                if( c == '\n' )
                {
                    if( i == 0 ) break;

                    buff[i]='\0';
                    data = std::string( (char *)buff );
                    delete buff;
                    return i;
                }
                buff[i++] = c;
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
        delete buff;
        return -1;
    }

    bool LineProtocol::sendData( std::string data )
    {
        std::string linea( data + "\n" );

        return sendbytes( (const unsigned char* )linea.c_str(), linea.length() );
    }

    bool LineProtocol::sendData( BinaryData* data )
    {
        unsigned int len = data->length();

        // los primeros 4 bytes serán el largo (BIG ENDIAN) de la data
        unsigned char buff[4];
        buff[0] = 0;
        buff[1] = ( len & 0x00FF0000 ) >> 16;
        buff[2] = ( len & 0x0000FF00 ) >> 8;
        buff[3] = ( len & 0x000000FF );

        // enviamos la data
        if( sendbytes( buff, 4 ) )
            if( sendbytes( data->getData(), len ) )
                return true;
        return false;
    }

    bool LineProtocol::sendbytes( const unsigned char buff[], unsigned int len )
    {
        unsigned int i = 0;
        while( i < len )
        {
            int n = send( sock, buff+i, len-i, MSG_NOSIGNAL );
            if( n <= 0) return false;
            i += n;
        }
        return true;
    }

}
