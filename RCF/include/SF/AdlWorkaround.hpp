
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_ADLWORKAROUND_HPP
#define INCLUDE_SF_ADLWORKAROUND_HPP

#include <SF/Archive.hpp>

#if defined(_MSC_VER) && _MSC_VER < 1310

#define SF_ADL_WORKAROUND(Ns, T)                                            \
namespace SF {                                                              \
    inline void serialize(Archive &ar, Ns::T &t, const unsigned int ver)    \
    {                                                                       \
        Ns::serialize(ar, t, ver);                                          \
    }                                                                       \
}

#else

#define SF_ADL_WORKAROUND(Ns, T)

#endif

#endif // ! INCLUDE_SF_ADLWORKAROUND_HPP
