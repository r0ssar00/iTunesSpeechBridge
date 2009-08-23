
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_UTIL_TRACETARGETFILEROLLOVER_HPP
#define INCLUDE_UTIL_TRACETARGETFILEROLLOVER_HPP

#include "Trace.hpp"

namespace util {

    class TraceTargetFileRollOver : public TraceTarget
    {
    public:
        TraceTargetFileRollOver(
            const std::string &filename,
            std::size_t maxSize = 10*1024*1024,
            std::size_t frequency = 100);
        bool isNull();
        std::string getName();
        void trace(const std::string &msg);

    private:
        Mutex m;
        std::auto_ptr<std::ofstream> fout;
        std::string filename;
        std::size_t counter;
        std::size_t fileCounter;
        const std::size_t maxSize;
        const std::size_t frequency;
    };

    // here's some test code will have the file rolling over frequently
    //{
    //    std::string traceChannelName = "RCF";
    //    std::string fileName = "trace.txt";
    //    boost::shared_ptr<util::TraceTarget> traceTargetPtr( new util::TraceTargetFileRollOver(fileName));
    //    util::TraceManager::getSingleton().makeTraceTarget(fileName, traceTargetPtr);
    //    util::TraceManager::getSingleton().getTraceChannel(traceChannelName).setTraceTarget(fileName);

    //    while (true)
    //    {
    //        int a = 1;
    //        int b = 2;
    //        int c = 3;
    //        RCF_TRACE("this is a nice little log message")(a)(b)(c);
    //    }
    //}

} // namespace util

#endif // INCLUDE_UTIL_TRACETARGETFILEROLLOVER_HPP
