
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_RCF_WIN32NAMEDPIPEENDPOINT_HPP
#define INCLUDE_RCF_WIN32NAMEDPIPEENDPOINT_HPP

#include <RCF/Endpoint.hpp>
#include <RCF/Export.hpp>
#include <RCF/ClientTransport.hpp>
#include <RCF/ServerTransport.hpp>

#include <SF/SerializeParent.hpp>

#include <RCF/util/Tchar.hpp>
#include <tchar.h>

namespace RCF {

    class RCF_EXPORT Win32NamedPipeEndpoint : public I_Endpoint
    {
    public:

        Win32NamedPipeEndpoint();

        Win32NamedPipeEndpoint(const tstring & pipeName);

        ServerTransportAutoPtr createServerTransport() const;
        ClientTransportAutoPtr createClientTransport() const;
        EndpointPtr clone() const;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            serializeParent( (I_Endpoint*) 0, ar, *this);
            ar & mPipeName;
        }

    private:
        tstring mPipeName;
    };

    RCF_EXPORT std::pair<tstring, HANDLE> generateNewPipeName();

} // namespace RCF

RCF_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(RCF::Win32NamedPipeEndpoint)

#endif // ! INCLUDE_RCF_WIN32NAMEDPIPEENDPOINT_HPP
