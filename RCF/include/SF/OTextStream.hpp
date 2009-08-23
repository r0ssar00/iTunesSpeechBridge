
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_OTEXTSTREAM_HPP
#define INCLUDE_SF_OTEXTSTREAM_HPP

#include <SF/Stream.hpp>

namespace SF {

    class OTextStream : public OStream, public WithEncodingText
    {
    public:
        OTextStream() : OStream()
        {}

        OTextStream(std::ostream &os) : OStream(os)
        {}
    };

}

#endif // ! INCLUDE_SF_OTEXTSTREAM_HPP
