
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

// Copyright (C) 2002-2003
// David Moore, William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#ifndef RCF_BOOST_BARRIER_JDM030602_HPP
#define RCF_BOOST_BARRIER_JDM030602_HPP

#include <boost/thread/detail/config.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace boost {

class RCF_BOOST_THREAD_DECL barrier
{
public:
    barrier(unsigned int count);
    ~barrier();

    bool wait();

private:
    mutex m_mutex;
    condition m_cond;
    unsigned int m_threshold;
    unsigned int m_count;
    unsigned int m_generation;
};

}   // namespace boost

#endif
