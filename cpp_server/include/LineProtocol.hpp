#ifndef LINEPROTOCOL_HPP
#define LINEPROTOCOL_HPP

#include "Connection.hpp"

#include <iostream>

namespace RobWorld
{
    class LineProtocol : public Connection
    {
        public:
            LineProtocol( int );
            virtual ~LineProtocol();
            virtual int readData( std::string&, unsigned int );
            virtual bool sendData( std::string );
            virtual bool sendData( BinaryData* );
    };
}

#endif // LINEPROTOCOL_HPP
