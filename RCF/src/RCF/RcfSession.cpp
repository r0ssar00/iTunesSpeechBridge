
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/RcfSession.hpp>

#include <RCF/ClientTransport.hpp>
#include <RCF/Marshal.hpp>
#include <RCF/PingBackService.hpp>
#include <RCF/RcfServer.hpp>
#include <RCF/SerializationProtocol.hpp>
#include <RCF/SessionTimeoutService.hpp>
#include <RCF/ThreadLocalCache.hpp>
#include <RCF/Version.hpp>

#include <boost/bind.hpp>

namespace RCF {

    RcfSession::RcfSession(RcfServer &server) :
        mStopCallInProgress(RCF_DEFAULT_INIT),
        mRcfServer(server),
        mRcfRuntimeVersion(RCF::getRuntimeVersion()),
        mTransportFiltersLocked(RCF_DEFAULT_INIT),
        mFiltered(RCF_DEFAULT_INIT),
        mCloseSessionAfterWrite(RCF_DEFAULT_INIT),
        mPingTimestamp(RCF_DEFAULT_INIT),
        mTouchTimestamp(Platform::OS::getCurrentTimeMs()),
        mIoState(Idle),
        mpParameters(RCF_DEFAULT_INIT),
        mAutoSend(RCF_DEFAULT_INIT)
    {}

    RcfSession::~RcfSession()
    {
        RCF_DTOR_BEGIN

            mRcfServer.unregisterSession(mWeakThisPtr);

            // no locks here, relying on dtor thread safety of reference counted objects
            clearParameters();
            if (mOnDestroyCallback)
            {
                mOnDestroyCallback(*this);
            }
        RCF_DTOR_END
    }

    void RcfSession::clearParameters()
    {
        if (mpParameters)
        {
            //mpParameters->~I_Parameters();

            // need to be elaborate here, for borland compiler
            typedef I_Parameters P;
            P &p = *mpParameters;
            p.~P();
            mpParameters = NULL;
        }

    }

    void RcfSession::setOnDestroyCallback(OnDestroyCallback onDestroyCallback)
    {
        Lock lock(mMutex);
        mOnDestroyCallback = onDestroyCallback;
    }

#ifdef RCF_USE_SF_SERIALIZATION

    void RcfSession::enableSfSerializationPointerTracking(bool enable)
    {
        mOut.mOutProtocol1.setCustomizationCallback(
            boost::bind(enableSfPointerTracking_1, _1, enable) );

        //mOut.mOutProtocol2.setCustomizationCallback(
        //    boost::bind(enableSfPointerTracking_2, _1, enable) );
    }

#else

    void RcfSession::enableSfSerializationPointerTracking(bool enable)
    {}

#endif

    void RcfSession::addOnWriteCompletedCallback(
        const OnWriteCompletedCallback &onWriteCompletedCallback)
    {
        Lock lock(mMutex);
        mOnWriteCompletedCallbacks.push_back(onWriteCompletedCallback);
    }

    void RcfSession::extractOnWriteCompletedCallbacks(
        std::vector<OnWriteCompletedCallback> &onWriteCompletedCallbacks)
    {
        Lock lock(mMutex);
        onWriteCompletedCallbacks.clear();
        onWriteCompletedCallbacks.swap( mOnWriteCompletedCallbacks );
    }

    const RCF::I_RemoteAddress &RcfSession::getRemoteAddress()
    {
        return getProactor().getRemoteAddress();
    }

    void RcfSession::disconnect()
    {
        getProactor().setEnableReconnect(false);
        getProactor().postClose();
    }

    bool RcfSession::hasDefaultServerStub()
    {
        Lock lock(mMutex);
        return mDefaultStubEntryPtr;
    }

    StubEntryPtr RcfSession::getDefaultStubEntryPtr()
    {
        Lock lock(mMutex);
        return mDefaultStubEntryPtr;
    }

    void RcfSession::setDefaultStubEntryPtr(const StubEntryPtr &stubEntryPtr)
    {
        Lock lock(mMutex);
        mDefaultStubEntryPtr = stubEntryPtr;
    }

    void RcfSession::setCachedStubEntryPtr(const StubEntryPtr &stubEntryPtr)
    {
        mCachedStubEntryPtr = stubEntryPtr;
    }

    void RcfSession::getMessageFilters(std::vector<FilterPtr> &filters)
    {
        filters = mFilters;
    }

    void RcfSession::getTransportFilters(std::vector<FilterPtr> &filters)
    {
        getProactor().getTransportFilters(filters);
    }

    int RcfSession::getRcfRuntimeVersion()
    {
        return mRcfRuntimeVersion;
    }

    void RcfSession::setRcfRuntimeVersion(int version)
    {
        mRcfRuntimeVersion = version;
    }

    void RcfSession::setUserData(boost::any userData)
    {
        mUserData = userData;
    }

    boost::any RcfSession::getUserData()
    {
        return mUserData;
    }

    void RcfSession::lockTransportFilters()
    {
        mTransportFiltersLocked = true;
    }

    void RcfSession::unlockTransportFilters()
    {
        mTransportFiltersLocked = false;
    }

    bool RcfSession::transportFiltersLocked()
    {
        return mTransportFiltersLocked;
    }

    SerializationProtocolIn & RcfSession::getSpIn()
    {
        return mIn;
    }

    SerializationProtocolOut & RcfSession::getSpOut()
    {
        return mOut;
    }

    bool RcfSession::getFiltered()
    {
        return mFiltered;
    }

    void RcfSession::setFiltered(bool filtered)
    {
        mFiltered = filtered;
    }

    std::vector<FilterPtr> & RcfSession::getFilters()
    {
        return mFilters;
    }

    RcfServer & RcfSession::getRcfServer()
    {
        return mRcfServer;
    }

    void RcfSession::setCloseSessionAfterWrite(bool close)
    {
        mCloseSessionAfterWrite = close;
    }

    boost::uint32_t RcfSession::getPingBackIntervalMs()
    {
        return mRequest.getPingBackIntervalMs();
    }

    boost::uint32_t RcfSession::getPingTimestamp()
    {
        Lock lock(mMutex);
        return mPingTimestamp;
    }

    boost::uint32_t RcfSession::getTouchTimestamp()
    {
        Lock lock(mMutex);
        return mTouchTimestamp;
    }

    void RcfSession::touch()
    {
        Lock lock(mMutex);
        mTouchTimestamp = Platform::OS::getCurrentTimeMs();
    }

    void RcfSession::registerForPingBacks()
    {
        // Register for ping backs if appropriate.

        if (    mRequest.getPingBackIntervalMs() > 0 
            &&  !mRequest.getOneway())
        {
            PingBackServicePtr pbsPtr = mRcfServer.getPingBackServicePtr();
            if (pbsPtr)
            {
                // Disable reconnecting for this session. After sending a 
                // pingback, a server I/O thread would get a write completion 
                // notification, and if it happened to be an error (very unlikely 
                // but possible), we definitely would not want a reconnect, as 
                // the session would still in use.
                getProactor().setEnableReconnect(false);

                PingBackTimerEntry pingBackTimerEntry = 
                    pbsPtr->registerSession(shared_from_this());

                Lock lock(mMutex);
                RCF_ASSERT( mPingBackTimerEntry.first == 0 );
                mPingBackTimerEntry = pingBackTimerEntry;
            }
            else
            {
                // TODO: something more efficient than throwing
                RCF_THROW( Exception(RcfError_NoPingBackService) );
            }
        }
    }

    void RcfSession::unregisterForPingBacks()
    {
        // Unregister for ping backs if appropriate.

        if (    mRequest.getPingBackIntervalMs() > 0 
            &&  !mRequest.getOneway())
        {
            PingBackTimerEntry pingBackTimerEntry;
            {
                Lock lock(mMutex);
                pingBackTimerEntry = mPingBackTimerEntry;
                mPingBackTimerEntry = PingBackTimerEntry();
            }

            if ( pingBackTimerEntry.second != RcfSessionWeakPtr() )
            {
                PingBackServicePtr pbsPtr = mRcfServer.getPingBackServicePtr();
                if (pbsPtr)
                {
                    pbsPtr->unregisterSession(pingBackTimerEntry);
                }
            }
        }
    }

    bool RcfSession::sendPingBack(
        const PingBackTimerEntry & timerEntry,
        boost::uint32_t pingBackIntervalMs)
    {
        {
            Lock lock(mMutex);
            if ( mPingBackTimerEntry.second == RcfSessionWeakPtr() )
            {
                return false;
            }
            else
            {
                mPingBackTimerEntry = timerEntry;
            }
        }

        bool okToWrite = false;
        {
            Lock lock(mIoStateMutex);
            if (mIoState == Idle)
            {
                mIoState = WritingPingBack;
                okToWrite = true;
            }
        }

        if (okToWrite)
        {
            ThreadLocalCached< std::vector<ByteBuffer> > tlcByteBuffers;
            std::vector<ByteBuffer> &byteBuffers = tlcByteBuffers.get();

            byteBuffers.push_back(mPingBackByteBuffer);

            encodeServerError(
                byteBuffers.front(),
                RcfError_PingBack,
                pingBackIntervalMs);
            
            getProactor().postWrite(byteBuffers);
            RCF_ASSERT(byteBuffers.empty());
        }

        return true;
    }

    bool RcfSession::getAutoSend()
    {
        return mAutoSend;
    }

    void RcfSession::setWeakThisPtr()
    {
        mWeakThisPtr = shared_from_this();
    }

    void RcfSession::dropReceiveBuffer()
    {
        getProactor().setDropReceiveBuffer(true);
    }

} // namespace RCF
