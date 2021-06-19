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
#include <cstring>
#include <openssl/sha.h>

#include "Connection.hpp"

namespace RobWorld
{

    BinaryData::BinaryData( unsigned char*_data, unsigned int _len ) :
        data( _data ), len( _len )
    {
    }

    BinaryData::~BinaryData()
    {
        if( data != nullptr ) delete data;
    }


    Connection::Connection( int _sock ) :
        sock( _sock ), isWebsocket( false )
    {
    }

    Connection::~Connection()
    {
    }

    bool Connection::doHandshake( unsigned int timeout )
    {
        char c;
        auto start = std::chrono::high_resolution_clock::now();
        while( true )
        {
            // si no ha llegado nada en los ultimos 'timeout' segundos marcamos error
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout ) break;

            int n = recv( sock, &c, 1, MSG_PEEK );
            if( n == 1 ) return c == '{' ? true : ws_doHandshake( timeout );
            if( n == 0 ) break;

            #ifdef WIN32
            if( WSAGetLastError() ==  WSAETIMEDOUT ) continue;
            #else
            if( errno == EAGAIN ) continue;
            #endif

            break;
        }
        return false;
    }

    int Connection::readline( char* buff, unsigned int bufflen, unsigned int timeout )
    {
        if( isWebsocket ) return ws_readline( buff, bufflen, timeout );

        char c;
        unsigned int i = 0;
        auto start = std::chrono::high_resolution_clock::now();
        while( i < bufflen )
        {
            // si no ha llegado nada en los ultimos 'timeout' segundos retornamos
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout ) return -i;

            int n = recv( sock, &c, 1, 0 );
            if( n == 1 )
            {
                if( c == '\n' )
                {
                    if( i == 0 ) break;

                    buff[i]='\0';
                    //std::cout << buff << std::endl;
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
        return -1;
    }

    bool Connection::sendline( std::string text )
    {
        if( isWebsocket ) return ws_sendline( text );

        unsigned char nl = '\n';

        if( sendbytes( (const unsigned char* )text.c_str(), text.length() ) )
            if( sendbytes( &nl, 1 ) )
                return true;
        return false;
    }

    bool Connection::sendbinarydata( BinaryData* bindata )
    {
        if( isWebsocket ) return ws_sendbinarydata( bindata );

        // los primeros 4 bytes serán el largo (BIG ENDIAN) de la data
        unsigned char buff[4];
        buff[0] = 0;
        buff[1] = ( bindata->len & 0x00FF0000 ) >> 16;
        buff[2] = ( bindata->len & 0x0000FF00 ) >> 8;
        buff[3] = ( bindata->len & 0x000000FF );

        // enviamos la data
        if( sendbytes( buff, 4 ) )
            if( sendbytes( bindata->data, bindata->len ) )
                return true;
        return false;
    }

    bool Connection::ws_doHandshake( unsigned int timeout )
    {
        std::string sec;
        char buff[128];
        bool checkFirst = true;
        auto start = std::chrono::high_resolution_clock::now();
        while( true )
        {
            auto end = std::chrono::high_resolution_clock::now();
            if( std::chrono::duration_cast<std::chrono::seconds>(end - start).count() >= timeout ) break;

            int n = readline( buff, sizeof(buff)/sizeof(buff[0]), timeout );
            if( n <= 0 ) break;
            if( n == 1 )
            {
                if( buff[0] != '\r' ) break;
                if( !sendline( "HTTP/1.1 101 Switching Protocols\r" ) ) break;
                if( !sendline( "Upgrade: websocket\r" ) ) break;
                if( !sendline( "Connection: Upgrade\r" ) ) break;
                if( !sendline( "Sec-WebSocket-Accept: " + std::string( sec ) + "\r" ) ) break;
                if( !sendline( "\r" ) ) break;
                isWebsocket = true;
                return true;
            }
            buff[strlen(buff)-1] = 0;
            std::string header( buff );
            if( checkFirst )
            {
                if( header.substr( 0, 6 ).compare( "GET / " ) != 0 ) break;
                checkFirst = false;
                continue;
            }
            if( header.substr( 0, 19 ).compare( "Sec-WebSocket-Key: " ) != 0 ) continue;
            std::string key( buff + 19 );

            std::string magic( key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );

            char hash[SHA_DIGEST_LENGTH*2];
            SHA1( (const unsigned char*)magic.c_str(), magic.length(), (unsigned char*)hash );

            std::string s_hash( hash );
            sec = base64_encode( s_hash );
        }
        return false;
    }

    int Connection::ws_readline( char* buff, unsigned int bufflen, unsigned int timeout )
    {
        return -1;
    }

    bool Connection::ws_sendline( std::string text )
    {
        return false;
    }

    bool Connection::ws_sendbinarydata( BinaryData* bindata )
    {
        return false;
    }

    int Connection::ws_readFrameData( std::string& data )
    {
        unsigned int state = 0;
        unsigned int fin = 0;
        unsigned int opcode = 0;
        unsigned int mask = 0;
        unsigned int plen = 0;
        unsigned char c;
        unsigned int idx = 0;
        unsigned char* buff = 0;
        while( true )
        {
            int n = recv( sock, &c, 1, 0 );
            if( n == 0 ) break;
            if( n < 0 )
            {
                #ifdef WIN32
                if( WSAGetLastError() ==  WSAETIMEDOUT ) continue;
                #else
                if( errno == EAGAIN ) continue;
                #endif
                break;
            }
            switch( state )
            {
                // byte 0
                case 0: fin = c & 0x01;
                        opcode = ( c & 0xF0 ) >> 4;
                        state = 1;
                        break;
                // byte 1 (aceptaremos solo data de hasta 125 bytes)
                case 1: mask = c & 0x01;
                        if( mask == 0 ) return -1;
                        plen = ( c & 0xFE ) >> 1;
                        if( plen > 125 ) return -1;
                        state++;
                        break;
                // masking key
                case 2: mask = c;
                        state++;
                        break;
                case 3: mask = ( (unsigned int)c << 8 ) | mask;
                        state++;
                        break;
                case 4: mask = ( (unsigned int)c << 16 ) | mask;
                        state++;
                        break;
                case 5: mask = ( (unsigned int)c << 24 ) | mask;
                        if( plen == 0 ) return -1;
                        buff = new unsigned char[plen];
                        idx = 0;
                        state++;
                        break;
                // data
                case 6: buff[idx++] = c;
                        if( idx == plen )
                        {
                            data = std::string( (char *)buff );
                            return 1;
                        }
                        break;
            }
        }
        if( buff != 0 ) delete buff;
        return -1;
    }

    std::string Connection::base64_encode( const std::string& in )
    {
        std::string out;

        int val = 0, valb = -6;
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
                valb -= 6;
            }
        }
        if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
        while (out.size()%4) out.push_back('=');
        return out;
    }

    bool Connection::sendbytes( const unsigned char buff[], unsigned int len )
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
