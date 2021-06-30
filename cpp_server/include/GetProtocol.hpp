#ifndef GETPROTOCOL_HPP
#define GETPROTOCOL_HPP

#include "Connection.hpp"

#include <iostream>
#include <vector>

namespace RobWorld
{
    class GetProtocol : public Connection
    {
        private:
            std::vector<std::string> content;
            std::string response;

        public:
            GetProtocol( int, std::string, std::string, std::vector<std::string> );
            virtual ~GetProtocol();
            virtual int readData( std::string&, unsigned int );
            virtual bool sendData( std::string );
            virtual bool sendData( BinaryData* );
    };
}

#endif // GETPROTOCOL_HPP
