
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_UTIL_PLATFORM_MACHINE_X86_BYTEORDER_HPP
#define INCLUDE_UTIL_PLATFORM_MACHINE_X86_BYTEORDER_HPP

namespace Platform {

    namespace Machine {

        class BigEndian {};
        class LittleEndian {};
        typedef LittleEndian ByteOrder;

    } // namespace Machine

} // namespace Platform

#endif // ! INCLUDE_UTIL_PLATFORM_MACHINE_X86_BYTEORDER_HPP
