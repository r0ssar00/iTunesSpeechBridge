
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_DEQUE_HPP
#define INCLUDE_SF_DEQUE_HPP

#include <deque>

#include <SF/SerializeStl.hpp>

namespace SF {

    // std::deque is not guaranteed to use contiguous storage, so even if T 
    // is a fundamental type, we can't do fast memcpy-style serialization, as
    // we do for std::vector.

    // std::deque
    template<typename T, typename A>
    inline void serialize(SF::Archive &ar, std::deque<T,A> &t, const unsigned int version)
    {
        serializeStlContainer<PushBackSemantics>(ar, t, version);
    }

} // namespace SF

#endif // ! INCLUDE_SF_DEQUE_HPP
