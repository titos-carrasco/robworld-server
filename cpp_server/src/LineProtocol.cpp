#include "BinaryData.hpp"
#include "LineProtocol.hpp"

#include <chrono>

namespace RobWorld
{
    LineProtocol::LineProtocol( int _sock )
    {
        sock = _sock;
    }

    LineProtocol::~LineProtocol()
    {
    }

    int LineProtocol::readData( std::string& data, unsigned int timeout )
    {
        return Connection::readline( sock, data, timeout );
    }

    bool LineProtocol::sendData( std::string data )
    {
        return Connection::sendline( sock, data );
    }

    bool LineProtocol::sendData( BinaryData* data )
    {
        return Connection::sendbinarydata( sock, data );
    }

}
