#include "BinaryData.hpp"
#include "GetProtocol.hpp"

//{"cmd":"setLedsIntensity","leds":["%20+%20"]}

namespace RobWorld
{
    GetProtocol::GetProtocol( int _sock, std::string method, std::string path, std::vector<std::string> headers )
    {
        bool err = true;
        size_t pos;

        sock = _sock;
        while( ( pos = path.find( "%22" ) ) != std::string::npos )
            path.replace( pos, 3, "\"" );

        // GET /nombre_robot?{...json...}
        if( method.compare( "GET" ) == 0 )
        {
            pos = path.find( "?" );
            if( pos != std::string::npos && pos >= 2 && pos < path.length() )
            {
                std::string robot = path.substr( 1, pos - 1 );
                content.push_back( "{\"connect\":\"" + robot + "\"}" );

                content.push_back( path.substr( pos + 1 ) );
            }
        }

        if( err )
            content.push_back( "{}" );
    }

    GetProtocol::~GetProtocol()
    {
        std::string all;

        if( response.length() == 0 )
            all = "HTTP/1.1 400 Bad Request\r\n";
        else
            all = "HTTP/1.1 200 OK\r\n";
        all = all +
              "Access-Control-Allow-Origin: *\r\n"
              "Access-Control-Allow-Headers: X-Requested-With, X-Application\r\n"
              "Content-Type: text/plain; charset=iso-8859-1\r\n"
              "Content-Length: ";
        response = "[" + response + "]";
        all = all + std::to_string( response.length() ) + "\r\n\r\n" + response;
        Connection::sendbytes( sock, (const unsigned char*)all.c_str(), all.length() );
    }

    int GetProtocol::readData( std::string& data, unsigned int timeout )
    {
        if( content.empty() ) return -1;

        data = content[0];
        content.erase( content.begin() );

        //std::cout << data << std::endl;
        return data.length();
    }

    bool GetProtocol::sendData( std::string data )
    {
        if( response.length() == 0 )
            response = data;
        else
            response = response + "," + data;
        return true;
    }

    bool GetProtocol::sendData( BinaryData* data )
    {
        return false;
    }

}
