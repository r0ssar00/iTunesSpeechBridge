
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/TcpEndpoint.hpp>

#include <boost/config.hpp>

#include <RCF/InitDeinit.hpp>
#include <RCF/SerializationProtocol.hpp>

#ifdef RCF_USE_BOOST_ASIO

#include <RCF/TcpAsioServerTransport.hpp>
#include <RCF/TcpClientTransport.hpp>

#elif defined(BOOST_WINDOWS)

#include <RCF/TcpIocpServerTransport.hpp>
#include <RCF/TcpClientTransport.hpp>

#else

#include <RCF/TcpClientTransport.hpp>

#endif

#ifdef RCF_USE_SF_SERIALIZATION
#include <SF/Registry.hpp>
#endif

namespace RCF {

    TcpEndpoint::TcpEndpoint() :
        mIp(),
        mPort(RCF_DEFAULT_INIT)
    {}

    TcpEndpoint::TcpEndpoint(int port) :
        mIp("127.0.0.1"),
        mPort(port)
    {}

    TcpEndpoint::TcpEndpoint(const std::string &ip, int port) :
        mIp(ip),
        mPort(port)
    {}

    TcpEndpoint::TcpEndpoint(const TcpEndpoint &rhs) :
        mIp(rhs.mIp),
        mPort(rhs.mPort)
    {}

    EndpointPtr TcpEndpoint::clone() const
    {
        return EndpointPtr(new TcpEndpoint(*this));
    }

    std::string TcpEndpoint::getIp() const
    {
        return mIp;
    }

    int TcpEndpoint::getPort() const
    {
        return mPort;
    }

#ifdef RCF_USE_BOOST_ASIO

    std::auto_ptr<I_ServerTransport> TcpEndpoint::createServerTransport() const
    {
        return std::auto_ptr<I_ServerTransport>(
            new RCF::TcpAsioServerTransport(mIp, mPort));
    }

    std::auto_ptr<I_ClientTransport> TcpEndpoint::createClientTransport() const
    {
        return std::auto_ptr<I_ClientTransport>(
            new RCF::TcpClientTransport(mIp, mPort));
    }

#elif defined(BOOST_WINDOWS)

    std::auto_ptr<I_ServerTransport> TcpEndpoint::createServerTransport() const
    {
        return std::auto_ptr<I_ServerTransport>(
            new RCF::TcpIocpServerTransport(mIp, mPort));
    }

    std::auto_ptr<I_ClientTransport> TcpEndpoint::createClientTransport() const
    {
        return std::auto_ptr<I_ClientTransport>(
            new RCF::TcpClientTransport(mIp, mPort));
    }

#else

    std::auto_ptr<I_ServerTransport> TcpEndpoint::createServerTransport() const
    {
        // On non Windows platforms, server side RCF code requires 
        // RCF_USE_BOOST_ASIO to be defined, and the Boost.Asio library to 
        // be available.
        RCF_ASSERT(0);
        return std::auto_ptr<I_ServerTransport>();
    }

    std::auto_ptr<I_ClientTransport> TcpEndpoint::createClientTransport() const
    {
        return std::auto_ptr<I_ClientTransport>(
            new RCF::TcpClientTransport(mIp, mPort));
    }

#endif

    inline void initTcpEndpointSerialization()
    {
#ifdef RCF_USE_SF_SERIALIZATION
        SF::registerType( (TcpEndpoint *) 0, "RCF::TcpEndpoint");
        SF::registerBaseAndDerived( (I_Endpoint *) 0, (TcpEndpoint *) 0);
#endif
    }

    RCF_ON_INIT_NAMED( initTcpEndpointSerialization(), InitTcpEndpointSerialization );

} // namespace RCF
