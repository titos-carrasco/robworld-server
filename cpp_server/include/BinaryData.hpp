#ifndef BINARYDATA_HPP
#define BINARYDATA_HPP

#include <iostream>

namespace RobWorld
{
    class BinaryData
    {
        private:
            unsigned char* data;
            unsigned int len;

        public:
            BinaryData( unsigned char[], unsigned int );
            ~BinaryData();
            const unsigned char* getData();
            unsigned int length();
    };
}

#endif // BINARYDATA_HPP
