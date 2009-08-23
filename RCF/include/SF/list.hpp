
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_LIST_HPP
#define INCLUDE_SF_LIST_HPP

#include <list>

#include <SF/SerializeStl.hpp>

namespace SF {

    // std::list
    template<typename T, typename A>
    inline void serialize(SF::Archive &ar, std::list<T,A> &t, const unsigned int version)
    {
        serializeStlContainer<PushBackSemantics>(ar, t, version);
    }

} // namespace SF

#endif // ! INCLUDE_SF_LIST_HPP
