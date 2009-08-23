
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/ClientStub.hpp>

#include <boost/bind.hpp>

#include <RCF/ClientProgress.hpp>
#include <RCF/ClientTransport.hpp>
#include <RCF/FileTransferService.hpp>
#include <RCF/InitDeinit.hpp>
#include <RCF/IpClientTransport.hpp>
#include <RCF/Marshal.hpp>
#include <RCF/SerializationProtocol.hpp>
#include <RCF/ServerInterfaces.hpp>
#include <RCF/Version.hpp>

#include <sys/stat.h>

namespace RCF {

    //****************************************
    // ClientStub

    // 2s default connect timeout
    static unsigned int gClientConnectTimeoutMs = 1000*2;

    // 10s default call timeout
    static unsigned int gClientRemoteCallTimeoutMs = 1000*10;
    
    void setDefaultConnectTimeoutMs(unsigned int connectTimeoutMs)
    {
        gClientConnectTimeoutMs = connectTimeoutMs;
    }

    unsigned int getDefaultConnectTimeoutMs()
    {
        return gClientConnectTimeoutMs;
    }

    void setDefaultRemoteCallTimeoutMs(unsigned int remoteCallTimeoutMs)
    {
        gClientRemoteCallTimeoutMs = remoteCallTimeoutMs;
    }

    unsigned int getDefaultRemoteCallTimeoutMs()
    {
        return gClientRemoteCallTimeoutMs;
    }

    void ClientStub::setAutoReconnect(bool autoReconnect)
    {
        mAutoReconnect = autoReconnect;
    }

    bool ClientStub::getAutoReconnect() const
    {
        return mAutoReconnect;
    }

    void ClientStub::setClientProgressPtr(ClientProgressPtr ClientProgressPtr)
    {
        mClientProgressPtr = ClientProgressPtr;
    }

    ClientProgressPtr ClientStub::getClientProgressPtr() const
    {
        return mClientProgressPtr;
    }

    ClientStub::ClientStub(const std::string &interfaceName) :
        mToken(),
        mDefaultCallingSemantics(Twoway),
        mProtocol(DefaultSerializationProtocol),
        mEndpointName(),
        mObjectName(),
        mInterfaceName(interfaceName),
        mRemoteCallTimeoutMs(gClientRemoteCallTimeoutMs),
        mConnectTimeoutMs(gClientConnectTimeoutMs),
        mAutoReconnect(true),
        mConnected(RCF_DEFAULT_INIT),
        mTries(RCF_DEFAULT_INIT),
        mAutoVersioning(true),
        mRcfRuntimeVersion(RCF::getRuntimeVersion()),

        mAsync(RCF_DEFAULT_INIT),
        mAsyncTimerReason(None),
        mEndTimeMs(RCF_DEFAULT_INIT),
        mRetry(RCF_DEFAULT_INIT),
        mRcs(Twoway),
        mEncodedByteBuffer(),
        mEncodedByteBuffers(),
        mpParameters(RCF_DEFAULT_INIT),
        mPingBackIntervalMs(RCF_DEFAULT_INIT),
        mPingBackTimeStamp(RCF_DEFAULT_INIT),
        mPingBackCount(RCF_DEFAULT_INIT),
        mNextTimerCallbackMs(RCF_DEFAULT_INIT),
        mNextPingBackCheckMs(RCF_DEFAULT_INIT),
        mPingBackCheckIntervalMs(RCF_DEFAULT_INIT),
        mTimerIntervalMs(RCF_DEFAULT_INIT),

        mSignalled(RCF_DEFAULT_INIT)
    {
    }

    ClientStub::ClientStub(const std::string &interfaceName, const std::string &objectName) :
        mToken(),
        mDefaultCallingSemantics(Twoway),
        mProtocol(DefaultSerializationProtocol),
        mEndpointName(),
        mObjectName(objectName),
        mInterfaceName(interfaceName),
        mRemoteCallTimeoutMs(gClientRemoteCallTimeoutMs),
        mConnectTimeoutMs(gClientConnectTimeoutMs),
        mAutoReconnect(true),
        mConnected(RCF_DEFAULT_INIT),
        mTries(RCF_DEFAULT_INIT),
        mAutoVersioning(true),
        mRcfRuntimeVersion(RCF::getRuntimeVersion()),
        
        mAsync(RCF_DEFAULT_INIT),
        mAsyncTimerReason(None),
        mEndTimeMs(RCF_DEFAULT_INIT),
        mRetry(RCF_DEFAULT_INIT),
        mRcs(Twoway),
        mEncodedByteBuffer(),
        mEncodedByteBuffers(),
        mpParameters(RCF_DEFAULT_INIT),
        mPingBackIntervalMs(RCF_DEFAULT_INIT),
        mPingBackTimeStamp(RCF_DEFAULT_INIT),
        mPingBackCount(RCF_DEFAULT_INIT),
        mNextTimerCallbackMs(RCF_DEFAULT_INIT),
        mNextPingBackCheckMs(RCF_DEFAULT_INIT),
        mPingBackCheckIntervalMs(RCF_DEFAULT_INIT),
        mTimerIntervalMs(RCF_DEFAULT_INIT),

        mSignalled(RCF_DEFAULT_INIT)
    {
    }

    ClientStub::ClientStub(const ClientStub &rhs) :
        mToken(rhs.mToken),
        mDefaultCallingSemantics(rhs.mDefaultCallingSemantics),
        mProtocol(rhs.mProtocol),
        mEndpointName(rhs.mEndpointName),
        mObjectName(rhs.mObjectName),
        mInterfaceName(rhs.mInterfaceName),
        mRemoteCallTimeoutMs(rhs.mRemoteCallTimeoutMs),
        mConnectTimeoutMs(rhs.mConnectTimeoutMs),
        mAutoReconnect(rhs.mAutoReconnect),
        mConnected(RCF_DEFAULT_INIT),
        mTries(RCF_DEFAULT_INIT),
        mAutoVersioning(rhs.mAutoVersioning),
        mRcfRuntimeVersion(rhs.mRcfRuntimeVersion),
        mUserData(rhs.mUserData),
        
        mAsync(RCF_DEFAULT_INIT),
        mAsyncTimerReason(None),
        mEndTimeMs(RCF_DEFAULT_INIT),
        mRetry(RCF_DEFAULT_INIT),
        mRcs(Twoway),
        mEncodedByteBuffer(),
        mEncodedByteBuffers(),
        mpParameters(RCF_DEFAULT_INIT),
        mPingBackIntervalMs(rhs.mPingBackIntervalMs),
        mPingBackTimeStamp(RCF_DEFAULT_INIT),
        mPingBackCount(RCF_DEFAULT_INIT),
        mNextTimerCallbackMs(RCF_DEFAULT_INIT),
        mNextPingBackCheckMs(RCF_DEFAULT_INIT),
        mPingBackCheckIntervalMs(RCF_DEFAULT_INIT),
        mTimerIntervalMs(RCF_DEFAULT_INIT),

        mSignalled(RCF_DEFAULT_INIT)
    {
        setEndpoint( rhs.getEndpoint() );
        if (rhs.mClientProgressPtr)
        {
            mClientProgressPtr.reset(
                new ClientProgress(*rhs.mClientProgressPtr));
        }
    }

    ClientStub::~ClientStub()
    {
        disconnect();
        clearParameters();        
    }

    void ClientStub::clearParameters()
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

    ClientStub &ClientStub::operator=( const ClientStub &rhs )
    {
        if (&rhs != this)
        {
            mInterfaceName              = rhs.mInterfaceName;
            mToken                      = rhs.mToken;
            mDefaultCallingSemantics    = rhs.mDefaultCallingSemantics;
            mProtocol                   = rhs.mProtocol;
            mEndpointName               = rhs.mEndpointName;
            mObjectName                 = rhs.mObjectName;
            mRemoteCallTimeoutMs        = rhs.mRemoteCallTimeoutMs;
            mConnectTimeoutMs           = rhs.mConnectTimeoutMs;
            mAutoReconnect              = rhs.mAutoReconnect;
            mConnected                  = false;
            mAutoVersioning             = rhs.mAutoVersioning;
            mRcfRuntimeVersion          = rhs.mRcfRuntimeVersion;
            mUserData                   = rhs.mUserData;
            mPingBackIntervalMs         = rhs.mPingBackIntervalMs;
            mSignalled                  = false;

            setEndpoint( rhs.getEndpoint());

            if (rhs.mClientProgressPtr)
            {
                mClientProgressPtr.reset(
                    new ClientProgress(*rhs.mClientProgressPtr));
            }
        }
        return *this;
    }

    Token ClientStub::getTargetToken() const
    {
        return mToken;
    }

    void ClientStub::setTargetToken(Token token)
    {
        mToken = token;
    }

    std::string ClientStub::getTargetName() const
    {
        return mObjectName;
    }

    void ClientStub::setTargetName(const std::string &objectName)
    {
        mObjectName = objectName;
    }

    RemoteCallSemantics ClientStub::getDefaultCallingSemantics() const
    {
        return mDefaultCallingSemantics;
    }

    void ClientStub::setDefaultCallingSemantics(
        RemoteCallSemantics defaultCallingSemantics)
    {
        mDefaultCallingSemantics = defaultCallingSemantics;
    }

    void ClientStub::setSerializationProtocol(int protocol)
    {
        mProtocol = protocol;
    }

    int ClientStub::getSerializationProtocol() const
    {
        return mProtocol;
    }

#ifdef RCF_USE_SF_SERIALIZATION

    void ClientStub::enableSfSerializationPointerTracking(bool enable)
    {
        mOut.mOutProtocol1.setCustomizationCallback(
            boost::bind(enableSfPointerTracking_1, _1, enable) );

        //mOut.mOutProtocol2.setCustomizationCallback(
        //    boost::bind(enableSfPointerTracking_2, _1, enable) );
    }

#else

    void ClientStub::enableSfSerializationPointerTracking(bool enable)
    {}

#endif

    void ClientStub::setEndpoint(const I_Endpoint &endpoint)
    {
        mEndpoint = endpoint.clone();
    }

    void ClientStub::setEndpoint(EndpointPtr endpointPtr)
    {
        mEndpoint = endpointPtr;
    }

    EndpointPtr ClientStub::getEndpoint() const
    {
        return mEndpoint;
    }

    void ClientStub::setTransport(std::auto_ptr<I_ClientTransport> transport)
    {
        mTransport = transport;
        mConnected = mTransport.get() && mTransport->isConnected();
    }

    std::auto_ptr<I_ClientTransport> ClientStub::releaseTransport()
    {
        instantiateTransport();
        return mTransport;
    }

    I_ClientTransport& ClientStub::getTransport()
    {
        instantiateTransport();
        return *mTransport;
    }

    I_IpClientTransport &ClientStub::getIpTransport()
    {
        return dynamic_cast<I_IpClientTransport &>(getTransport());
    }

    void ClientStub::instantiateTransport()
    {
        if (!mTransport.get())
        {
            RCF_VERIFY(mEndpoint.get(), Exception(RcfError_NoEndpoint));
            mTransport.reset( mEndpoint->createClientTransport().release() );
            RCF_VERIFY(mTransport.get(), Exception(RcfError_TransportCreation));
        }
    }

    void ClientStub::disconnect()
    {
        RcfClientPtr subRcfClientPtr = getSubRcfClientPtr();
        setSubRcfClientPtr( RcfClientPtr() );
        if (subRcfClientPtr)
        {
            subRcfClientPtr->getClientStub().disconnect();
            subRcfClientPtr.reset();
        }

        if (mTransport.get())
        {
            mTransport->disconnect(mConnectTimeoutMs);
            mConnected = false;
        }

        mAsyncCallback = boost::function0<void>();
    }

    bool ClientStub::isConnected()
    {
        return mTransport.get() && mTransport->isConnected();
    }

    void ClientStub::setMessageFilters()
    {
        setMessageFilters( std::vector<FilterPtr>());
    }

    void ClientStub::setMessageFilters(const std::vector<FilterPtr> &filters)
    {
        mMessageFilters.assign(filters.begin(), filters.end());
        RCF::connectFilters(mMessageFilters);
    }

    void ClientStub::setMessageFilters(FilterPtr filterPtr)
    {
        std::vector<FilterPtr> filters;
        filters.push_back(filterPtr);
        setMessageFilters(filters);
    }

    const std::vector<FilterPtr> &ClientStub::getMessageFilters()
    {
        return mMessageFilters;
    }

    void ClientStub::setRemoteCallTimeoutMs(unsigned int remoteCallTimeoutMs)
    {
        mRemoteCallTimeoutMs = remoteCallTimeoutMs;
    }

    unsigned int ClientStub::getRemoteCallTimeoutMs() const
    {
        return mRemoteCallTimeoutMs;
    }

    void ClientStub::setConnectTimeoutMs(unsigned int connectTimeoutMs)
    {
        mConnectTimeoutMs = connectTimeoutMs;
    }

    unsigned int ClientStub::getConnectTimeoutMs() const
    {
        return mConnectTimeoutMs;
    }

    void ClientStub::setAutoVersioning(bool autoVersioning)
    {
        mAutoVersioning = autoVersioning;
    }

    bool ClientStub::getAutoVersioning() const
    {
        return mAutoVersioning;
    }

    void ClientStub::setRcfRuntimeVersion(int version)
    {
        mRcfRuntimeVersion = version;
    }

    int ClientStub::getRcfRuntimeVersion() const
    {
        return mRcfRuntimeVersion;
    }

    void ClientStub::setTries(std::size_t tries)
    {
        mTries = tries;
    }

    std::size_t ClientStub::getTries() const
    {
        return mTries;
    }

    CurrentClientStubSentry::CurrentClientStubSentry(ClientStubPtr clientStubPtr)
    {
        pushCurrentClientStubPtr(clientStubPtr);
    }

    CurrentClientStubSentry::~CurrentClientStubSentry()
    {
        popCurrentClientStubPtr();
    }

    void ClientStub::onError(const std::exception &e)
    {
        const RemoteException *pRcfRE = 
            dynamic_cast<const RemoteException *>(&e);

        const Exception *pRcfE = 
            dynamic_cast<const Exception *>(&e);

        boost::function0<void> cb = mAsyncCallback;
        mAsyncCallback = boost::function0<void>();

        if (pRcfRE)
        {
            setAsyncException(pRcfRE->clone());
        }
        else if (pRcfE)
        {
            disconnect();
            setAsyncException(pRcfE->clone());
        }
        else
        {
            disconnect();
            setAsyncException( std::auto_ptr<Exception>(
                new Exception(e.what())));
        }

        {
            Lock lock(*mSignalledMutex);
            mSignalled = true;
            mSignalledCondition->notify_all();
        }

        if (mTransport.get() && mAsyncTimerEntry != TimerEntry())
        {
            mTransport->killTimer(mAsyncTimerEntry);
            mAsyncTimerEntry = TimerEntry();
            mAsyncTimerReason = None;
        }
        
        if (cb)
        {
            cb();
        }
    }

    void ClientStub::onTimerExpired()
    {
        TimerReason timerReason = mAsyncTimerReason;
        mAsyncTimerReason = None;

        if (timerReason == Wait)
        {
            boost::function0<void> cb = mAsyncCallback;
            mAsyncCallback = boost::function0<void>();

            {
                Lock lock(*mSignalledMutex);
                mSignalled = true;
                mSignalledCondition->notify_all();
            }

            if (mTransport.get() && mAsyncTimerEntry != TimerEntry())
            {
                mTransport->killTimer(mAsyncTimerEntry);
                mAsyncTimerEntry = TimerEntry();
            }

            if (cb)
            {
                cb();
            }

        }
        else
        {
            switch(timerReason)
            {
            case Connect:
                onError(RCF::Exception(RcfError_ClientConnectTimeout));
                break;

            case Write:
                onError(RCF::Exception(RcfError_ClientWriteTimeout));
                break;

            case Read: 
                onError(RCF::Exception(RcfError_ClientReadTimeout));
                break;

            default:
                RCF_ASSERT(0)(timerReason);
            };
        }        
    }

    void ClientStub::setUserData(boost::any userData)
    {
        mUserData = userData;
    }

    boost::any ClientStub::getUserData()
    {
        return mUserData;
    }

    void ClientStub::setInterfaceName(const std::string & interfaceName)
    {
        mInterfaceName = interfaceName;
    }

    std::string ClientStub::getInterfaceName()
    {
        return mInterfaceName;
    }

    SerializationProtocolIn & ClientStub::getSpIn()
    {
        return mIn;
    }

    SerializationProtocolOut & ClientStub::getSpOut()
    {
        return mOut;
    }

    void ClientStub::setPingBackIntervalMs(int pingBackIntervalMs)
    {
        mPingBackIntervalMs = pingBackIntervalMs;
    }
    
    int ClientStub::getPingBackIntervalMs()
    {
        return mPingBackIntervalMs;
    }

    std::size_t ClientStub::getPingBackCount()
    {
        return mPingBackCount;
    }

    boost::uint32_t ClientStub::getPingBackTimeStamp()
    {
        return mPingBackTimeStamp;
    }

    void ClientStub::ping()
    {
        ping( getDefaultCallingSemantics() );
    }

    void ClientStub::ping(RemoteCallSemantics rcs)
    {
        typedef Void V;

        CurrentClientStubSentry sentry(shared_from_this());

        AllocateClientParameters<V,V,V,V,V,V,V,V,V >::ParametersT & parms = 
            AllocateClientParameters<V,V,V,V,V,V,V,V,V >()(
                *this, V(), V(), V(), V(), V(), V(), V(), V());

        FutureImpl<V>(
            parms.r.get(),
            *this,
            mInterfaceName,
            -1,
            CallOptions(rcs).apply(*this));
    }   

    // Take the proposed timeout and cut it down to accommodate client progress 
    // callbacks and checking of ping back interval.

    boost::uint32_t ClientStub::generatePollingTimeout(boost::uint32_t timeoutMs)
    {
        boost::uint32_t timeNowMs = Platform::OS::getCurrentTimeMs();

        boost::uint32_t timeToNextTimerCallbackMs = mNextTimerCallbackMs ?
            mNextTimerCallbackMs - timeNowMs:
            -1;

        boost::uint32_t timeToNextPingBackCheckMs = mNextPingBackCheckMs ?
            mNextPingBackCheckMs - timeNowMs:
            -1;

        return 
            RCF_MIN( 
                RCF_MIN(timeToNextTimerCallbackMs, timeToNextPingBackCheckMs), 
                timeoutMs);
    }

    void ClientStub::onPollingTimeout()
    {
        // Check whether we need to fire a client progress timer callback.
        if (mNextTimerCallbackMs && 0 == generateTimeoutMs(mNextTimerCallbackMs))
        {
            ClientProgress::Action action = ClientProgress::Continue;

            mClientProgressPtr->mProgressCallback(
                0,
                0,
                ClientProgress::Timer,
                ClientProgress::Receive,
                action);

            RCF_VERIFY(
                action == ClientProgress::Continue,
                Exception(RcfError_ClientCancel))
                (mTimerIntervalMs);

            mNextTimerCallbackMs = 
                Platform::OS::getCurrentTimeMs() + mTimerIntervalMs;

            mNextTimerCallbackMs |= 1;
        }

        // Check that pingbacks have been received.
        if (mNextPingBackCheckMs && 0 == generateTimeoutMs(mNextPingBackCheckMs))
        {
            boost::uint32_t timeNowMs = Platform::OS::getCurrentTimeMs();

            boost::uint32_t timeSinceLastPingBackMs = 
                timeNowMs - mPingBackTimeStamp;

            RCF_VERIFY(
                timeSinceLastPingBackMs < mPingBackCheckIntervalMs,
                Exception(RcfError_PingBackTimeout)) // TODO: special error for pingbacks
                (mPingBackCheckIntervalMs);

            mNextPingBackCheckMs = 
                Platform::OS::getCurrentTimeMs() + mPingBackCheckIntervalMs;

            mNextPingBackCheckMs |= 1;
        }

    }

    void ClientStub::onUiMessage()
    {
        ClientProgress::Action action = ClientProgress::Continue;

        mClientProgressPtr->mProgressCallback(
            0,
            0,
            ClientProgress::UiMessage,
            ClientProgress::Receive,
            action);

        RCF_VERIFY(
            action != ClientProgress::Cancel,
            Exception(RcfError_ClientCancel))
            (mClientProgressPtr->mUiMessageFilter);

        // a sample message filter

        //MSG msg = {0};
        //while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        //{
        //    if (msg.message == WM_QUIT)
        //    {
        //
        //    }
        //    else if (msg.message == WM_PAINT)
        //    {
        //        TranslateMessage(&msg);
        //        DispatchMessage(&msg);
        //    }
        //}

    }

} // namespace RCF

namespace RCF {

    // TODO: resuming of failed transfers, in either direction.

    void ClientStub::uploadFile(
        const std::string & whichFile,
        boost::uint32_t chunkSize)
    {
        uploadFile(NULL, whichFile, chunkSize);
    }
    
    void ClientStub::uploadFile(
        Token & token,
        const std::string & whichFile,
        boost::uint32_t chunkSize)
    {
        uploadFile(&token, whichFile, chunkSize);
    }
    
    void ClientStub::uploadFile(
        Token * pToken,
        const std::string & whichFile,
        boost::uint32_t chunkSize)
    {
        ClientStub & clientStub = *this;

        if (! clientStub.isConnected())
        {
            clientStub.connect();
        }

        if (pToken)
        {
            *pToken = Token();
        }

        RCF::RcfClient<RCF::I_FileTransferService> ftsClient(clientStub);
        ftsClient.getClientStub().setTransport( clientStub.releaseTransport() );
        ftsClient.getClientStub().setTargetToken( Token());

        RestoreClientTransportGuard guard(clientStub, ftsClient.getClientStub());
        RCF_UNUSED_VARIABLE(guard);

        // Upload chunks to the server until we're done.
        const boost::uint64_t FileSize = fileSize(whichFile);
        const boost::uint32_t ChunkSize = chunkSize;
        ByteBuffer byteBuffer(ChunkSize);

        std::ifstream fin( whichFile.c_str(), std::ios::binary);
        RCF_VERIFY(fin, Exception(RcfError_FileOpen))(whichFile);

        boost::uint64_t pos = 0;        
        while (fin && pos < FileSize)
        {
            std::size_t bytesRead =
                fin.read( byteBuffer.getPtr(), byteBuffer.getLength()).gcount();

            RCF_VERIFY(!fin.fail() || fin.eof(), Exception(RcfError_FileRead));

            if (bytesRead)
            {
                boost::uint32_t err = RcfError_Ok;
                if (pToken)
                {
                    err = ftsClient.uploadChunk(
                        *pToken, 
                        pos,
                        ByteBuffer(byteBuffer, 0, bytesRead),
                        FileSize);
                }
                else
                {
                    err = ftsClient.uploadChunk(
                        pos,
                        ByteBuffer(byteBuffer, 0, bytesRead),
                        FileSize);
                }

                pos += bytesRead;

                RCF_VERIFY(static_cast<int>(err) == RcfError_Ok, RemoteException(err));
            }            
        }
    }

    void ClientStub::downloadFile(
        const std::string & whichFile,
        boost::uint32_t chunkSize,
        bool resumeExisting)
    {
        downloadFile(NULL, whichFile, chunkSize, resumeExisting);    
    }

    void ClientStub::downloadFile(
        Token token, 
        const std::string & whichFile,
        boost::uint32_t chunkSize,
        bool resumeExisting)
    {
        downloadFile(&token, whichFile, chunkSize, resumeExisting);
    }

    void ClientStub::downloadFile(
        Token * pToken, 
        const std::string & whichFile,
        boost::uint32_t chunkSize,
        bool resumeExisting)
    {
        ClientStub & clientStub = *this;

        if (! clientStub.isConnected())
        {
            clientStub.connect();
        }

        RCF::RcfClient<RCF::I_FileTransferService> ftsClient(clientStub);
        ftsClient.getClientStub().setTransport( clientStub.releaseTransport() );
        ftsClient.getClientStub().setTargetToken( Token());

        RestoreClientTransportGuard guard(clientStub, ftsClient.getClientStub());
        RCF_UNUSED_VARIABLE(guard);

        // Download chunks from the server until we're done.

        std::ios::openmode mode = resumeExisting ? 
            std::ios::binary | std::ios::app :
            std::ios::binary | std::ios::trunc;

        boost::uint64_t pos = 0;

        if (resumeExisting)
        {
            pos = fileSize(whichFile);
        }

        std::ofstream fout( whichFile.c_str(), mode);
        RCF_VERIFY(fout, Exception(RcfError_FileOpen))(whichFile);
        const boost::uint32_t ChunkSize = chunkSize;
        ByteBuffer byteBuffer;

        while (true)
        {
            boost::uint64_t fileSize = 0;

            boost::uint32_t err = RcfError_Ok;

            if (pToken)
            {
                err = ftsClient.downloadChunk(
                    *pToken, 
                    pos, 
                    ChunkSize, 
                    byteBuffer,
                    fileSize);
            }
            else
            {
                err = ftsClient.downloadChunk(
                    pos, 
                    ChunkSize, 
                    byteBuffer,
                    fileSize);                
            }

            RCF_VERIFY(static_cast<int>(err) == RcfError_Ok, RemoteException(err));

            if (byteBuffer)
            {
                fout.write( byteBuffer.getPtr(), byteBuffer.getLength() );
                RCF_VERIFY(!fout.fail(), Exception(RcfError_FileWrite));

                pos += byteBuffer.getLength();
            }

            if (pos == fileSize)
            {
                fout.close();
                break;
            }
        }
    }

    //**************************************************************************
    // Synchronous create object calls.

    namespace {

        void reinstateClientTransport(
            ClientStub &clientStub,
            I_RcfClient &factory)
        {
            clientStub.setTransport(factory.getClientStub().releaseTransport());
        }

    }

    void ClientStub::createRemoteObject(
        const std::string &objectName_)
    {
        const std::string &objectName = objectName_.empty() ? mInterfaceName : objectName_;
        unsigned int timeoutMs = getRemoteCallTimeoutMs();
        connect();
        RcfClient<I_ObjectFactory> factory(*this);
        factory.getClientStub().setTransport( releaseTransport());
        factory.getClientStub().setTargetToken( Token());
        // TODO: should only be using the remainder of the timeout
        factory.getClientStub().setRemoteCallTimeoutMs(timeoutMs);
        using namespace boost::multi_index::detail;
        scope_guard guard = make_guard(
            reinstateClientTransport,
            boost::ref(*this),
            boost::ref(factory));
        RCF_UNUSED_VARIABLE(guard);
        RCF::Token token;
        boost::int32_t ret = factory.createObject(RCF::Twoway, objectName, token);
        if (ret == RcfError_Ok)
        {
            setTargetToken(token);
        }
        else
        {
            setTargetToken(Token());

            // dtor issues with borland
#ifdef __BORLANDC__
            setTransport(factory.getClientStub().releaseTransport());
            guard.dismiss();
#endif

            RCF_THROW( RemoteException(ret));
        }
    }

    // ObjectFactoryClient is an abstraction of RcfClient<I_ObjectFactoryService>,
    // and RcfClient<I_SessionObjectFactoryService>. We need to use either one,
    // depending on what the RCF runtime version is.

    class ObjectFactoryClient
    {
    public:
        ObjectFactoryClient(ClientStub & clientStub) :
            mRcfRuntimeVersion(clientStub.getRcfRuntimeVersion()),
            mCutoffVersion(2)
        {
            mRcfRuntimeVersion <= mCutoffVersion ?
                client1.reset( new RcfClient<I_ObjectFactory>(clientStub)) :
                client2.reset( new RcfClient<I_SessionObjectFactory>(clientStub));
        }

        ClientStub &getClientStub()
        {
            return mRcfRuntimeVersion <= mCutoffVersion ?
                client1->getClientStub() :
                client2->getClientStub();
        }

        RcfClientPtr getRcfClientPtr()
        {
            return mRcfRuntimeVersion <= mCutoffVersion ?
                RcfClientPtr(client1) :
                RcfClientPtr(client2);
        }

        FutureImpl<boost::int32_t> createSessionObject(
            const ::RCF::CallOptions &callOptions,
            const std::string & objectName)
        {
            return mRcfRuntimeVersion <= mCutoffVersion ?
                client1->createSessionObject(callOptions, objectName) :
                client2->createSessionObject(callOptions, objectName);
        }

        FutureImpl<boost::int32_t> deleteSessionObject(
            const ::RCF::CallOptions &callOptions)
        {
            return mRcfRuntimeVersion <= mCutoffVersion ?
                client1->deleteSessionObject(callOptions) :
                client2->deleteSessionObject(callOptions);
        }

        void reinstateClientTransport(ClientStub & clientStub)
        {
            ClientTransportAutoPtr clientTransportAutoPtr = 
                mRcfRuntimeVersion <= mCutoffVersion ?
                    client1->getClientStub().releaseTransport() :
                    client2->getClientStub().releaseTransport();

            clientStub.setTransport(clientTransportAutoPtr);
        }

    private:
        boost::shared_ptr<RcfClient<I_ObjectFactory> >          client1;
        boost::shared_ptr<RcfClient<I_SessionObjectFactory> >   client2;

        const int                                               mRcfRuntimeVersion;
        const int                                               mCutoffVersion;
    };

    void ClientStub::createRemoteSessionObject(
        const std::string &objectName_)
    {
        const std::string &objectName = objectName_.empty() ? mInterfaceName : objectName_;
        unsigned int timeoutMs = getRemoteCallTimeoutMs();
        connect();
        
        ObjectFactoryClient factory(*this);
        
        factory.getClientStub().setTransport( releaseTransport());
        factory.getClientStub().setTargetToken( Token());
        // TODO: should only be using the remainder of the timeout
        factory.getClientStub().setRemoteCallTimeoutMs(timeoutMs);

        using namespace boost::multi_index::detail;
        scope_guard guard = make_obj_guard(
            factory,
            &ObjectFactoryClient::reinstateClientTransport,
            boost::ref(*this));
        RCF_UNUSED_VARIABLE(guard);

        boost::int32_t ret = factory.createSessionObject(RCF::Twoway, objectName);
        if (ret == RcfError_Ok)
        {
            setTargetName("");
            setTargetToken(Token());
        }
        else
        {
            RCF_THROW( RemoteException(ret));
        }
    }

    void ClientStub::deleteRemoteSessionObject()
    {
        ObjectFactoryClient factory(*this);
        factory.getClientStub().setTransport( releaseTransport());
        factory.getClientStub().setTargetToken( Token());

        using namespace boost::multi_index::detail;
        scope_guard guard = make_obj_guard(
            factory,
            &ObjectFactoryClient::reinstateClientTransport,
            boost::ref(*this));
        RCF_UNUSED_VARIABLE(guard);

        boost::int32_t ret = factory.deleteSessionObject(RCF::Twoway);
        RCF_VERIFY(ret == RcfError_Ok, RCF::RemoteException(ret));
    }

    void ClientStub::deleteRemoteObject()
    {
        Token token = getTargetToken();
        if (token != Token())
        {
            RcfClient<I_ObjectFactory> factory(*this);
            factory.getClientStub().setTransport( releaseTransport());
            factory.getClientStub().setTargetToken( Token());
            using namespace boost::multi_index::detail;
            scope_guard guard = make_guard(
                reinstateClientTransport,
                boost::ref(*this),
                boost::ref(factory));
            RCF_UNUSED_VARIABLE(guard);

            boost::int32_t ret = factory.deleteObject(RCF::Twoway, token);
            RCF_VERIFY(ret == RcfError_Ok, RCF::RemoteException(ret));
        }
    }

    //**************************************************************************
    // Synchronous transport filter requests.

    void pushBackFilterId(std::vector<boost::int32_t> &filterIds, FilterPtr filterPtr)
    {
        filterIds.push_back( filterPtr->getFilterDescription().getId());
    }

    // TODO: merge common code with queryTransportFilters()
    void ClientStub::requestTransportFilters(const std::vector<FilterPtr> &filters)
    {
        // TODO: the current message filter sequence is not being used,
        // when making the filter request call to the server.

        using namespace boost::multi_index::detail; // for scope_guard

        std::vector<boost::int32_t> filterIds;
        std::for_each(filters.begin(), filters.end(),
            boost::bind(pushBackFilterId, boost::ref(filterIds), _1));

        if (!isConnected())
        {
            connect();
        }
        RCF::RcfClient<RCF::I_RequestTransportFilters> client(*this);
        client.getClientStub().setTransport( releaseTransport());
        client.getClientStub().setTargetToken( Token());

        RestoreClientTransportGuard guard(*this, client.getClientStub());
        RCF_UNUSED_VARIABLE(guard);

        client.getClientStub().setRemoteCallTimeoutMs( getRemoteCallTimeoutMs() );
        int ret = client.requestTransportFilters(RCF::Twoway, filterIds);
        RCF_VERIFY(ret == RcfError_Ok, RemoteException(ret))(filterIds);
        client.getClientStub().getTransport().setTransportFilters(filters);
    }

    void ClientStub::requestTransportFilters(FilterPtr filterPtr)
    {
        std::vector<FilterPtr> filters;
        if (filterPtr.get())
        {
            filters.push_back(filterPtr);
        }
        requestTransportFilters(filters);
    }

    void ClientStub::requestTransportFilters()
    {
        requestTransportFilters( std::vector<FilterPtr>());
    }

    void ClientStub::clearTransportFilters()
    {
        disconnect();
        if (mTransport.get())
        {
            mTransport->setTransportFilters( std::vector<FilterPtr>());
        }
    }

    bool ClientStub::queryForTransportFilters(const std::vector<FilterPtr> &filters)
    {
        using namespace boost::multi_index::detail; // for scope_guard

        std::vector<boost::int32_t> filterIds;
        std::for_each(filters.begin(), filters.end(),
            boost::bind(pushBackFilterId, boost::ref(filterIds), _1));

        if (!isConnected())
        {
            connect();
        }
        RCF::RcfClient<RCF::I_RequestTransportFilters> client(*this);
        client.getClientStub().setTransport( releaseTransport());
        client.getClientStub().setTargetToken( Token());

        RestoreClientTransportGuard guard(*this, client.getClientStub());
        RCF_UNUSED_VARIABLE(guard);

        client.getClientStub().setRemoteCallTimeoutMs( getRemoteCallTimeoutMs() );
        int ret = client.queryForTransportFilters(RCF::Twoway, filterIds);
        return ret == RcfError_Ok;
    }

    bool ClientStub::queryForTransportFilters(FilterPtr filterPtr)
    {
        std::vector<FilterPtr> filters;
        if (filterPtr.get())
        {
            filters.push_back(filterPtr);
        }
        return queryForTransportFilters(filters);
    }

    //**************************************************************************
    // Asynchronous object creation/destruction.

    class Handler
    {
    public:

        virtual ~Handler()
        {
        }

        void handle(
            Future<boost::int32_t>      fRet,
            I_RcfClient &               rcfClient,
            ClientStub &                clientStubOrig,
            boost::function0<void>      onCompletion)
        {
            ClientStubPtr clientStubPtr = 
                rcfClient.getClientStub().shared_from_this();

            ClientStubPtr clientStubOrigPtr = clientStubOrig.shared_from_this();

            clientStubOrigPtr->setTransport( 
                clientStubPtr->releaseTransport() );

            clientStubOrigPtr->setSubRcfClientPtr( RcfClientPtr() );

            std::auto_ptr<Exception> ape(clientStubPtr->getAsyncException());

            bool failed = (ape.get() != NULL);

            clientStubOrigPtr->setAsyncException(ape);

            if (failed)
            {
                onCompletion();
            }
            else
            {
                mClientStubPtr = clientStubOrigPtr;

                boost::int32_t ret = fRet;
                if (ret == RcfError_Ok)
                {
                    handleOk();
                    onCompletion();
                }
                else
                {
                    std::auto_ptr<Exception> apException(
                        new RemoteException(ret));

                    clientStubOrigPtr->setAsyncException(apException);

                    handleFail();

                    onCompletion();
                }
            }
        }

        virtual void handleOk()
        {
        }

        virtual void handleFail()
        {
        }

    protected:
        ClientStubPtr mClientStubPtr;
    };

    typedef boost::shared_ptr<Handler> HandlerPtr;

    class CreateSessionObjectHandler : public Handler
    {
    private:
        void handleOk()
        {
            mClientStubPtr->setTargetName("");
            mClientStubPtr->setTargetToken(Token());
        }
    };

    class CreateObjectHandler : public Handler
    {
    public :
        CreateObjectHandler(Future<Token> fToken) :
            mfToken(fToken)
        {
        }

    private:
        void handleOk()
        {
            Token token = mfToken;
            mClientStubPtr->setTargetToken(token);
        }

        void handleFail()
        {
            mClientStubPtr->setTargetToken(Token());
        }

        Future<Token> mfToken;
    };

    class DeleteSessionObjectHandler : public Handler
    {};

    class DeleteObjectHandler : public Handler
    {};

    void ClientStub::createRemoteSessionObjectAsync(
        boost::function0<void>      onCompletion,
        const std::string &         objectName_)
    {
        const std::string &objectName = objectName_.empty() ? mInterfaceName : objectName_;
        unsigned int timeoutMs = getRemoteCallTimeoutMs();

        ObjectFactoryClient factory(*this);

        factory.getClientStub().setTransport( releaseTransport());
        factory.getClientStub().setTargetToken( Token());
        // TODO: should only be using the remainder of the timeout
        factory.getClientStub().setRemoteCallTimeoutMs(timeoutMs);

        setSubRcfClientPtr(factory.getRcfClientPtr());

        setAsync(true);

        Future<boost::int32_t> fRet;

        HandlerPtr handlerPtr( new CreateSessionObjectHandler());

        fRet = factory.createSessionObject(

            RCF::AsyncTwoway( boost::bind(
                &Handler::handle, 
                handlerPtr,
                fRet,
                boost::ref(*factory.getRcfClientPtr()),
                boost::ref(*this),
                onCompletion)),
                
            objectName);
    }

    void ClientStub::deleteRemoteSessionObjectAsync(
        boost::function0<void> onCompletion)
    {
        ObjectFactoryClient factory(*this);
        factory.getClientStub().setTransport( releaseTransport());
        factory.getClientStub().setTargetToken( Token());

        setSubRcfClientPtr(factory.getRcfClientPtr());

        setAsync(true);

        Future<boost::int32_t> fRet;

        HandlerPtr handlerPtr( new DeleteSessionObjectHandler());

        fRet = factory.deleteSessionObject(

            RCF::AsyncTwoway( boost::bind(
                &Handler::handle, 
                handlerPtr,
                fRet,
                boost::ref(*factory.getRcfClientPtr()),
                boost::ref(*this),
                onCompletion))
                
                );
    }

    void ClientStub::createRemoteObjectAsync(
        boost::function0<void>  onCompletion,
        const std::string &     objectName_)
    {
        const std::string &objectName = objectName_.empty() ? mInterfaceName : objectName_;
        unsigned int timeoutMs = getRemoteCallTimeoutMs();
        //connect();

        typedef RcfClient<I_ObjectFactory> OfClient;
        typedef boost::shared_ptr<OfClient> OfClientPtr;

        OfClientPtr ofClientPtr( new OfClient(*this) );
        ofClientPtr->getClientStub().setTransport( releaseTransport());
        ofClientPtr->getClientStub().setTargetToken( Token());
        // TODO: should only be using the remainder of the timeout
        ofClientPtr->getClientStub().setRemoteCallTimeoutMs(timeoutMs);

        setSubRcfClientPtr(ofClientPtr);

        setAsync(true);

        Future<boost::int32_t> fRet;
        Future<Token> fToken;

        HandlerPtr handlerPtr( new CreateObjectHandler(fToken));

        fRet = ofClientPtr->createObject(

            RCF::AsyncTwoway( boost::bind(
                &Handler::handle, 
                handlerPtr,
                fRet,
                boost::ref(*ofClientPtr),
                boost::ref(*this),
                onCompletion)),
                
            objectName,
            fToken);
    }

    void ClientStub::deleteRemoteObjectAsync(
        boost::function0<void> onCompletion)
    {
        Token token = getTargetToken();
        if (token != Token())
        {
            typedef RcfClient<I_ObjectFactory> OfClient;
            typedef boost::shared_ptr<OfClient> OfClientPtr;

            OfClientPtr ofClientPtr( new OfClient(*this) );
            ofClientPtr->getClientStub().setTransport( releaseTransport());
            ofClientPtr->getClientStub().setTargetToken( Token());

            setSubRcfClientPtr(ofClientPtr);

            setAsync(true);

            Future<boost::int32_t> fRet;

            HandlerPtr handlerPtr( new DeleteObjectHandler());

            fRet = ofClientPtr->deleteObject(

                RCF::AsyncTwoway( boost::bind(
                    &Handler::handle, 
                    handlerPtr,
                    fRet,
                    boost::ref(*ofClientPtr),
                    boost::ref(*this),
                    onCompletion)),
                    
                token);
        }
    }

    //**************************************************************************
    // Asynchronous transport filter requests.

    class RequestTransportFiltersHandler : public Handler
    {
    public :
        RequestTransportFiltersHandler(
            boost::shared_ptr< std::vector<FilterPtr> > filtersPtr) :
            mFiltersPtr(filtersPtr)
        {
        }

    private:
        void handleOk()
        {
            mClientStubPtr->getTransport().setTransportFilters(*mFiltersPtr);
        }

        boost::shared_ptr< std::vector<FilterPtr> > mFiltersPtr;
    };

    class QueryForTransportFiltersHandler : public Handler
    {
    };

    void ClientStub::requestTransportFiltersAsync(
        const std::vector<FilterPtr> &filters,
        boost::function0<void> onCompletion)
    {

        std::vector<boost::int32_t> filterIds;

        std::for_each(
            filters.begin(), 
            filters.end(),
            boost::bind(pushBackFilterId, boost::ref(filterIds), _1));

        boost::shared_ptr<std::vector<FilterPtr> > filtersPtr(
            new std::vector<FilterPtr>(filters) );

        typedef RcfClient<I_RequestTransportFilters> RtfClient;
        typedef boost::shared_ptr<RtfClient> RtfClientPtr;

        RtfClientPtr rtfClientPtr( new RtfClient(*this) );

        rtfClientPtr->getClientStub().setTransport( releaseTransport());
        rtfClientPtr->getClientStub().setTargetToken( Token());

        setSubRcfClientPtr(rtfClientPtr);

        setAsync(true);

        Future<boost::int32_t> fRet;

        HandlerPtr handlerPtr( new RequestTransportFiltersHandler(filtersPtr));

        fRet = rtfClientPtr->requestTransportFilters(
            
            RCF::AsyncTwoway( boost::bind(
                &Handler::handle, 
                handlerPtr,
                fRet,
                boost::ref(*rtfClientPtr),
                boost::ref(*this),
                onCompletion)),

            filterIds);

    }

    void ClientStub::queryForTransportFiltersAsync(
        const std::vector<FilterPtr> &filters,
        boost::function0<void> onCompletion)
    {

        std::vector<boost::int32_t> filterIds;

        std::for_each(filters.begin(), filters.end(),
            boost::bind(pushBackFilterId, boost::ref(filterIds), _1));

        typedef RcfClient<I_RequestTransportFilters> RtfClient;
        typedef boost::shared_ptr<RtfClient> RtfClientPtr;

        RtfClientPtr rtfClientPtr( new RtfClient(*this) );
        rtfClientPtr->getClientStub().setTransport( releaseTransport());
        rtfClientPtr->getClientStub().setTargetToken( Token());

        setSubRcfClientPtr(rtfClientPtr);

        setAsync(true);

        Future<boost::int32_t> fRet;

        HandlerPtr handlerPtr( new QueryForTransportFiltersHandler());

        fRet = rtfClientPtr->queryForTransportFilters(
            
            RCF::AsyncTwoway( boost::bind(
                &Handler::handle, 
                handlerPtr,
                fRet,
                boost::ref(*rtfClientPtr),
                boost::ref(*this),
                onCompletion)),

            filterIds);
    }

    void ClientStub::requestTransportFiltersAsync(
        FilterPtr filterPtr,
        boost::function0<void> onCompletion)
    {
        std::vector<FilterPtr> filters;
        if (filterPtr.get())
        {
            filters.push_back(filterPtr);
        }
        requestTransportFiltersAsync(filters, onCompletion);
    }

    void ClientStub::queryForTransportFiltersAsync(
        FilterPtr filterPtr,
        boost::function0<void> onCompletion)
    {
        std::vector<FilterPtr> filters;
        if (filterPtr.get())
        {
            filters.push_back(filterPtr);
        }
        queryForTransportFiltersAsync(filters, onCompletion);
    }

} // namespace RCF
