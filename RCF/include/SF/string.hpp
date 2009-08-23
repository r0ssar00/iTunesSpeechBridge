
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_STRING_HPP
#define INCLUDE_SF_STRING_HPP

#include <algorithm>
#include <string>

#include <SF/Archive.hpp>
#include <SF/Stream.hpp>

namespace SF {

    // std::basic_string
    template<typename C, typename T, typename A>
    inline void serialize(SF::Archive &ar, std::basic_string<C,T,A> &t, const unsigned int)
    {
        if (ar.isRead())
        {
            boost::uint32_t count = 0;
            ar & count;

            SF::IStream &is = dynamic_cast<SF::IStream &>(*ar.getStream());

            t.resize(0);

            std::size_t minSerializedLength = sizeof(C);
            if (ar.verifyAgainstArchiveSize(count*minSerializedLength))
            {
                t.reserve(count);
            }

            boost::uint32_t charsRemaining = count;
            const boost::uint32_t BufferSize = 512;
            C buffer[BufferSize];
            while (charsRemaining)
            {
                boost::uint32_t charsToRead = RCF_MIN(BufferSize, charsRemaining);
                boost::uint32_t bytesToRead = charsToRead*sizeof(C);

                RCF_VERIFY(
                    is.read( (char *) buffer, bytesToRead) == bytesToRead,
                    RCF::Exception(RCF::RcfError_Deserialization))
                    (bytesToRead)(BufferSize)(count);

                t.append(buffer, charsToRead);
                charsRemaining -= charsToRead;
            }
        }
        else if (ar.isWrite())
        {
            boost::uint32_t count = static_cast<boost::uint32_t >(t.length());
            ar & count;
            dynamic_cast<SF::OStream *>(ar.getStream())->writeRaw(
                (char *) t.c_str(),
                count*sizeof(C));
        }
    }

} // namespace SF

#endif // ! INCLUDE_SF_STRING_HPP
