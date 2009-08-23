
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_RCF_VERSION_HPP
#define INCLUDE_RCF_VERSION_HPP

#include <RCF/Export.hpp>

// RCF 0.9c - 903
// RCF 0.9d - 9040
// RCF 1.0 - 10000
// RCF 1.1 - 11000
#define RCF_VERSION 11000

namespace RCF {

    // legacy       - version number 1

    // 2007-04-26   - version number 2
    // Released in 0.9c

    // 2008-03-29   - version number 3
    //      - Using I_SessionObjectFactory instead of I_ObjectFactoryService for session object creation and deletion.
    // Released in 0.9d

    // 2008-09-06   - version number 4
    //      - ByteBuffer compatible with std::vector etc.
    // Released in 1.0

    // 2008-12-06   - version number 5
    //      - Pingback field in MethodInvocationRequest
    // Released in 1.1

    RCF_EXPORT int getRuntimeVersionInherent();
    RCF_EXPORT int getRuntimeVersion();
    RCF_EXPORT void setRuntimeVersion(int version);

    // In the context of a remote call (server side or client side), retrieve
    // the currently negotiated runtime version.
    RCF_EXPORT int getMyRuntimeVersion();


} // namespace RCF

#endif // ! INCLUDE_RCF_VERSION_HPP
