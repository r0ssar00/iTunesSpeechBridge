
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/UnixLocalEndpoint.hpp>

#include <RCF/InitDeinit.hpp>

#ifdef RCF_USE_SF_SERIALIZATION
#include <SF/Registry.hpp>
#endif

#include <RCF/UnixLocalServerTransport.hpp>
#include <RCF/UnixLocalClientTransport.hpp>

namespace RCF {

    UnixLocalEndpoint::UnixLocalEndpoint()
    {}

    UnixLocalEndpoint::UnixLocalEndpoint(const std::string & pipeName) :
            mPipeName(pipeName)
    {}

    ServerTransportAutoPtr UnixLocalEndpoint::createServerTransport() const
    {
        return ServerTransportAutoPtr(new UnixLocalServerTransport(mPipeName));
    }

    ClientTransportAutoPtr UnixLocalEndpoint::createClientTransport() const
    {            
        return ClientTransportAutoPtr(new UnixLocalClientTransport(mPipeName));
    }

    EndpointPtr UnixLocalEndpoint::clone() const
    {
        return EndpointPtr( new UnixLocalEndpoint(*this) );
    }

    inline void initUnixLocalEndpointSerialization()
    {
#ifdef RCF_USE_SF_SERIALIZATION
        SF::registerType( (UnixLocalEndpoint *) 0, "RCF::UnixLocalEndpoint");
        SF::registerBaseAndDerived( (I_Endpoint *) 0, (UnixLocalEndpoint *) 0);
#endif
    }

    RCF_ON_INIT_NAMED( initUnixLocalEndpointSerialization(), InitUnixLocalEndpointSerialization );

} // namespace RCF
