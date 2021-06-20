#ifndef LINEPROTOCOL_HPP
#define LINEPROTOCOL_HPP

#include <iostream>
#include "Connection.hpp"

#define MAXLINELENGTH   1024

namespace RobWorld
{
    class LineProtocol : public Connection
    {
        private:
            int sock;

        public:
            LineProtocol( int );
            ~LineProtocol();
            virtual int readData( std::string&, unsigned int );
            virtual bool sendData( std::string );
            virtual bool sendData( BinaryData* );

        private:
            bool sendbytes( const unsigned char[], unsigned int );
    };
}

#endif // LINEPROTOCOL_HPP
