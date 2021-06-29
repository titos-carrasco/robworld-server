#include <cstring>
#include "BinaryData.hpp"

namespace RobWorld
{
    BinaryData::BinaryData( unsigned char*_data, unsigned int _len )
    {
        len = _len;
        if( len == 0 ) data = nullptr;
        else
        {
            data = new unsigned char[len];
            memcpy( data, _data, len );
        }
    }

    BinaryData::~BinaryData()
    {
        if( data != nullptr ) delete[] data;
    }

    const unsigned char* BinaryData::getData()
    {
        return data;
    }

    unsigned int BinaryData::length()
    {
        return len;
    }
}
