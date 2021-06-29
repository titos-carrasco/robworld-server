#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <vector>
#include "BinaryData.hpp"

#define MAXLINELENGTH   1024

namespace RobWorld
{
    class Connection
    {
        protected:
            int sock;

        public:
            Connection();
            virtual ~Connection();
            static Connection* getConnector( int, unsigned int );
            virtual int readData( std::string&, unsigned int ) = 0;
            virtual bool sendData( std::string ) = 0;
            virtual bool sendData( BinaryData* ) = 0;

        protected:
            static int readline( int, std::string&, unsigned int );
            static int readbyte( int, unsigned char* );
            static bool sendline( int, std::string );
            static bool sendbinarydata( int, BinaryData* );
            static bool sendbytes( int, const unsigned char[], unsigned int );
    };
}

#endif // CONNECTION_HPP
