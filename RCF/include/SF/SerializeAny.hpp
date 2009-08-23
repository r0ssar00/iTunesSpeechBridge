
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_SERIALIZEANY_HPP
#define INCLUDE_SF_SERIALIZEANY_HPP

#include <boost/any.hpp>

namespace SF {

    class Archive;

    class I_SerializerAny
    {
    public:
        virtual ~I_SerializerAny() 
        {}

        virtual void serialize(
            SF::Archive &ar, 
            boost::any &a, 
            const unsigned int) = 0;
    };

    template<typename T>
    class SerializerAny : public I_SerializerAny
    {
    public:
        void serialize(SF::Archive &ar, boost::any &a, const unsigned int);
    };

} // namespace SF

#include <SF/Archive.hpp>

namespace SF {

    template<typename T>
    void SerializerAny<T>::serialize(SF::Archive &ar, boost::any &a, const unsigned int)
    {
        if (ar.isWrite())
        {
            ar & boost::any_cast<T>(a);
        }
        else
        {
            T t;
            ar & t;
            a = t;
        }
    }

} // namespace SF

#endif // ! INCLUDE_SF_SERIALIZEANY_HPP
