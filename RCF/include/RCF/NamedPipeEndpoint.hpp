
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_RCF_NAMEDPIPEENDPOINT_HPP
#define INCLUDE_RCF_NAMEDPIPEENDPOINT_HPP

#include <RCF/Endpoint.hpp>
#include <RCF/Export.hpp>
#include <RCF/ClientTransport.hpp>
#include <RCF/ServerTransport.hpp>
#include <RCF/Tools.hpp>

#include <SF/SerializeParent.hpp>

#include <boost/version.hpp>

#if !defined(BOOST_WINDOWS)
#if BOOST_VERSION < 103600
#error Need Boost 1.36.0 or later for Unix domain socket support.
#endif
#endif

namespace RCF {

    class RCF_EXPORT NamedPipeEndpoint : public I_Endpoint
    {
    public:

        NamedPipeEndpoint();

        NamedPipeEndpoint(const tstring & pipeName);

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

} // namespace RCF

RCF_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(RCF::NamedPipeEndpoint)

#endif // ! INCLUDE_RCF_NAMEDPIPEENDPOINT_HPP
