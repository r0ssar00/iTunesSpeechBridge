
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_UTILITY_HPP
#define INCLUDE_SF_UTILITY_HPP

#include <utility>

#include <SF/Archive.hpp>

namespace SF {

    // std::pair
    template<typename T, typename U>
    inline void serialize(Archive &ar, std::pair<T,U> &t, const unsigned int)
    {
        ar & t.first & t.second;
    }

} // namespace SF

#endif // ! INCLUDE_SF_UTILITY_HPP
