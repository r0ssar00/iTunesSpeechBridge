
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_ANY_HPP
#define INCLUDE_SF_ANY_HPP

#include <boost/any.hpp>

namespace SF {

    class Archive;

    void serialize(SF::Archive &ar, boost::any &a, const unsigned int version);

} // namespace SF

#endif // ! INCLUDE_SF_ANY_HPP
