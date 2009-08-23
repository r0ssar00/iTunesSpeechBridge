
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_IBINARYSTREAM_HPP
#define INCLUDE_SF_IBINARYSTREAM_HPP

#include <SF/Stream.hpp>

namespace SF {

    class IBinaryStream : public IStream, public WithEncodingBinaryPortable
    {
    public:
        IBinaryStream() : IStream()
        {}

        IBinaryStream(std::istream &is) : IStream(is)
        {}
    };

}

#endif // ! INCLUDE_SF_IBINARYSTREAM_HPP
