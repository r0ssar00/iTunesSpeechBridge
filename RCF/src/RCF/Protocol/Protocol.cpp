
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/Protocol/Protocol.hpp>
#include <RCF/SerializationDefs.hpp>
#include <RCF/SerializationProtocol.hpp>

namespace RCF {

#ifdef RCF_USE_SF_SERIALIZATION

    const int DefaultSerializationProtocol = SfBinary;

#else

    const int DefaultSerializationProtocol = BsBinary;

#endif

} // namespace RCF
