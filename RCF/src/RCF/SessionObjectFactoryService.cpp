
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/SessionObjectFactoryService.hpp>

#include <RCF/ServerInterfaces.hpp>

namespace RCF {

    void SessionObjectFactoryService::onServiceAdded(RcfServer &server)
    {
        server.bind((I_SessionObjectFactory *) NULL, *this);
    }

    void SessionObjectFactoryService::onServiceRemoved(RcfServer &server)
    {
        server.unbind( (I_SessionObjectFactory *) NULL);
    }

} // namespace RCF
