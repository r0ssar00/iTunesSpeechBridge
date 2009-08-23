
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_MAP_HPP
#define INCLUDE_SF_MAP_HPP

#include <map>

#include <SF/SerializeStl.hpp>
#include <SF/utility.hpp>

namespace SF {

    // std::map
    template<typename K, typename T1, typename T2, typename A>
    inline void serialize(Archive &ar, std::map<K,T1,T2,A> &t, const unsigned int version)
    {
        serializeStlContainer<InsertSemantics>(ar, t, version);
    }

    // std::multimap
    template<typename K, typename T1, typename T2, typename A>
    inline void serialize(Archive &ar, std::multimap<K,T1,T2,A> &t, const unsigned int version)
    {
        serializeStlContainer<InsertSemantics>(ar, t, version);
    }

}

#endif // ! INCLUDE_SF_MAP_HPP
