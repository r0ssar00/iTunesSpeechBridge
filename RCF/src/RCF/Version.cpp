
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/Version.hpp>

#include <RCF/ClientStub.hpp>
#include <RCF/Exception.hpp>
#include <RCF/RcfSession.hpp>
#include <RCF/ThreadLocalData.hpp>

namespace RCF {

    const int gRcfRuntimeVersionInherent = 5;

    int gRcfRuntimeVersion = gRcfRuntimeVersionInherent;

    int getRuntimeVersionInherent()
    {
        return gRcfRuntimeVersionInherent;
    }

    int getRuntimeVersion()
    {
        return gRcfRuntimeVersion;
    }

    void setRuntimeVersion(int version)
    {
        RCF_VERIFY(
            1 <= version && version <= gRcfRuntimeVersionInherent,
            Exception(RcfError_UnsupportedRuntimeVersion))
            (version)(gRcfRuntimeVersionInherent);

        gRcfRuntimeVersion = version;
    }

    int getMyRuntimeVersion()
    {
        RCF::ClientStubPtr clientStubPtr = RCF::getCurrentClientStubPtr();
        RCF::RcfSessionPtr rcfSessionPtr = RCF::getCurrentRcfSessionPtr();
        if (clientStubPtr)
        {
            return clientStubPtr->getRcfRuntimeVersion();
        }
        else if (rcfSessionPtr)
        {
            return rcfSessionPtr->getRcfRuntimeVersion();
        }
        else
        {
            return RCF::getRuntimeVersion();
        }
    }

} // namespace RCF
