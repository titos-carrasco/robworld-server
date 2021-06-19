#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>

namespace RobWorld
{
    class BinaryData
    {
        public:
            unsigned char* data;
            unsigned int len;

        public:
            BinaryData( unsigned char*, unsigned int );
            ~BinaryData();
    };


    class Connection
    {
        private:
            int sock;
            bool isWebsocket;

        public:
            Connection( int );
            ~Connection();
            bool doHandshake( unsigned int );
            int readline( char[], unsigned int, unsigned int );
            bool sendline( std::string );
            bool sendbinarydata( BinaryData* );

        private:
            bool ws_doHandshake( unsigned int );
            int ws_readline( char[], unsigned int, unsigned int );
            bool ws_sendline( std::string );
            bool ws_sendbinarydata( BinaryData* );

            int ws_readFrameData( std::string& );
            std::string base64_encode( const std::string& );
            bool sendbytes( const unsigned char[], unsigned int );

    };
}

#endif // CONNECTION_HPP
