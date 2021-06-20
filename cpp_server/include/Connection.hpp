#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include "BinaryData.hpp"

namespace RobWorld
{
    class Connection
    {
        public:
            Connection();
            virtual ~Connection();
            static Connection* getConnector( int, unsigned int );
            virtual int readData( std::string&, unsigned int ) = 0;
            virtual bool sendData( std::string ) = 0;
            virtual bool sendData( BinaryData* ) = 0;
    };
}

#endif // CONNECTION_HPP
