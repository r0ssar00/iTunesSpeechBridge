
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/FilterService.hpp>

#include <boost/bind.hpp>

#include <RCF/CurrentSession.hpp>
#include <RCF/RcfServer.hpp>
#include <RCF/RcfSession.hpp>
#include <RCF/ServerInterfaces.hpp>

namespace RCF {

    FilterService::FilterService() :
        mFilterFactoryMapMutex(WriterPriority)
    {
    }

    void FilterService::onServiceAdded(RcfServer &server)
    {
        server.bind( (I_RequestTransportFilters *) NULL, *this);
    }

    void FilterService::onServiceRemoved(RcfServer &)
    {
    }

    void FilterService::addFilterFactory(FilterFactoryPtr filterFactoryPtr)
    {
        FilterDescription filterDescription = filterFactoryPtr->getFilterDescription();
        WriteLock writeLock(mFilterFactoryMapMutex);
        mFilterFactoryMap[ filterDescription.getId() ] = filterFactoryPtr;
    }

    void FilterService::addFilterFactory(
        FilterFactoryPtr filterFactoryPtr,
        const std::vector<int> &filterIds)
    {
        FilterDescription filterDescription = filterFactoryPtr->getFilterDescription();
        WriteLock writeLock(mFilterFactoryMapMutex);
        for (std::size_t i=0; i<filterIds.size(); ++i)
        {
            mFilterFactoryMap[ filterIds[i] ] = filterFactoryPtr;
        }
    }

#if defined(_MSC_VER) && _MSC_VER <= 1200
#define for if (0) {} else for
#endif

    // remotely accessible
    boost::int32_t FilterService::requestTransportFilters(const std::vector<boost::int32_t> &filterIds)
    {
        ReadLock readLock(mFilterFactoryMapMutex);
        for (unsigned int i=0; i<filterIds.size(); ++i)
        {
            int filterId = filterIds[i];
            if (mFilterFactoryMap.find(filterId) == mFilterFactoryMap.end())
            {
                return RcfError_UnknownFilter;
            }
        }
        boost::shared_ptr< std::vector<FilterPtr> > filters(
            new std::vector<FilterPtr>());

        for (unsigned int i=0; i<filterIds.size(); ++i)
        {
            int filterId = filterIds[i];
            FilterFactoryPtr filterFactoryPtr = mFilterFactoryMap[filterId];
            FilterPtr filterPtr( filterFactoryPtr->createFilter() );
            filters->push_back(filterPtr);
        }
        RcfSession & session = getCurrentRcfSession();
        if (session.transportFiltersLocked())
        {
            return RcfError_FiltersLocked;
        }
        else
        {
            session.addOnWriteCompletedCallback( boost::bind(
                &FilterService::setTransportFilters, 
                this, 
                _1, 
                filters) );
        }        

        return RcfError_Ok;
    }

#if defined(_MSC_VER) && _MSC_VER <= 1200
#undef for
#endif

    // remotely accessible
    boost::int32_t FilterService::queryForTransportFilters(const std::vector<boost::int32_t> &filterIds)
    {
        ReadLock readLock(mFilterFactoryMapMutex);
        for (unsigned int i=0; i<filterIds.size(); ++i)
        {
            int filterId = filterIds[i];
            if (mFilterFactoryMap.find(filterId) == mFilterFactoryMap.end())
            {
                return RcfError_UnknownFilter;
            }
        }

        return getCurrentRcfSession().transportFiltersLocked() ?
            RcfError_FiltersLocked :
            RcfError_Ok;
    }

    FilterFactoryPtr FilterService::getFilterFactoryPtr(int filterId)
    {
        ReadLock readLock(mFilterFactoryMapMutex);
        return mFilterFactoryMap.find(filterId) == mFilterFactoryMap.end() ?
            FilterFactoryPtr() :
            mFilterFactoryMap[filterId];
    }

    void FilterService::setTransportFilters(
        RcfSession &session,
        boost::shared_ptr<std::vector<FilterPtr> > filters)
    {
        session.getProactor().setTransportFilters(*filters);
    }

} // namespace RCF
