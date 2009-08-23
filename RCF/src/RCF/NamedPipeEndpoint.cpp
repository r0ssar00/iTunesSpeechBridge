
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/NamedPipeEndpoint.hpp>

#include <RCF/InitDeinit.hpp>

#ifdef RCF_USE_SF_SERIALIZATION
#include <SF/Registry.hpp>
#endif

#if defined(BOOST_WINDOWS)
#include <RCF/Win32NamedPipeClientTransport.hpp>
#include <RCF/Win32NamedPipeServerTransport.hpp>
#else
#include <RCF/UnixLocalServerTransport.hpp>
#include <RCF/UnixLocalClientTransport.hpp>
#endif

namespace RCF {

    NamedPipeEndpoint::NamedPipeEndpoint()
    {}

    NamedPipeEndpoint::NamedPipeEndpoint(const tstring & pipeName) :
        mPipeName(pipeName)
    {}

#if defined(BOOST_WINDOWS)

    ServerTransportAutoPtr NamedPipeEndpoint::createServerTransport() const
    {
        return ServerTransportAutoPtr(
            new Win32NamedPipeServerTransport(mPipeName));
    }

    ClientTransportAutoPtr NamedPipeEndpoint::createClientTransport() const
    {            
        return ClientTransportAutoPtr(
            new Win32NamedPipeClientTransport(mPipeName));
    }

#else

    ServerTransportAutoPtr NamedPipeEndpoint::createServerTransport() const
    {
        return ServerTransportAutoPtr(new UnixLocalServerTransport(mPipeName));
    }

    ClientTransportAutoPtr NamedPipeEndpoint::createClientTransport() const
    {
        return ClientTransportAutoPtr(new UnixLocalClientTransport(mPipeName));
    }

#endif

    EndpointPtr NamedPipeEndpoint::clone() const
    {
        return EndpointPtr( new NamedPipeEndpoint(*this) );
    }

    inline void initNamedPipeEndpointSerialization()
    {
#ifdef RCF_USE_SF_SERIALIZATION
        SF::registerType( (NamedPipeEndpoint *) 0, "RCF::NamedPipeEndpoint");
        SF::registerBaseAndDerived( (I_Endpoint *) 0, (NamedPipeEndpoint *) 0);
#endif
    }

    RCF_ON_INIT_NAMED( initNamedPipeEndpointSerialization(), InitNamedPipeEndpointSerialization );

} // namespace RCF
