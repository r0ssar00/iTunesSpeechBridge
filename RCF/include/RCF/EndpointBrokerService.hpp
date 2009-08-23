
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_RCF_ENDPOINTBROKERSERVICE_HPP
#define INCLUDE_RCF_ENDPOINTBROKERSERVICE_HPP

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <RCF/ClientTransport.hpp>
#include <RCF/Export.hpp>
#include <RCF/RcfSession.hpp>
#include <RCF/ServerInterfaces.hpp>
#include <RCF/ServerTransport.hpp>
#include <RCF/Service.hpp>
#include <RCF/Tools.hpp>

namespace RCF {

    class RCF_EXPORT EndpointBroker
    {
    public:

        EndpointBroker(
            ServerTransportPtr serverTransportPtr,
            const std::string &endpointName,
            const std::string &endpointClientPassword,
            const std::string &endpointServerPassword);

        boost::int32_t connectToEndpoint();

    private:
        friend class EndpointBrokerService;
        typedef RcfClient<I_EndpointServer> Client;
        typedef boost::shared_ptr<Client> ClientPtr;
        std::string mEndpointName;
        std::string mEndpointServerPassword;
        std::string mEndpointClientPassword;
        std::vector<RcfSessionPtr> mConnections;
        ClientPtr mMasterConnection;
        ServerTransportPtr mServerTransportPtr;
    };

    typedef boost::shared_ptr<EndpointBroker> EndpointBrokerPtr;

    class RCF_EXPORT EndpointBrokerService :
        public I_Service,
        boost::noncopyable
    {
    public:

        EndpointBrokerService();

        void onServiceAdded(RcfServer &server);
        void onServiceRemoved(RcfServer &server);
        void onServerClose(RcfServer &server);

        // remotely invoked
        boost::int32_t openEndpoint(
            const std::string &endpointName,
            const std::string &endpointClientPassword,
            std::string &endpointServerPassword);

        boost::int32_t closeEndpoint(
            const std::string &endpointName,
            const std::string &endpointServerPassword);

        boost::int32_t establishEndpointConnection(
            const std::string &endpointName,
            const std::string &endpointServerPassword);

        boost::int32_t connectToEndpoint(
            const std::string &endpointName,
            const std::string &endpointClientPassword);

    private:
        ServerTransportPtr mServerTransportPtr;
        ReadWriteMutex mEndpointBrokersMutex;
        std::map<std::string, EndpointBrokerPtr> mEndpointBrokers;
    };

    typedef boost::shared_ptr<EndpointBrokerService> EndpointBrokerServicePtr;

} // namespace RCF

#endif // ! INCLUDE_RCF_ENDPOINTBROKERSERVICE_HPP
