
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/Exception.hpp>

#include <RCF/Tools.hpp>

#include <RCF/util/Platform/OS/BsdSockets.hpp> // GetErrorString()

#include <RCF/SerializationDefs.hpp>

#ifdef RCF_USE_SF_SERIALIZATION
#include <SF/Archive.hpp>
#include <SF/string.hpp>
#endif

namespace RCF {

    std::string getErrorString(int rcfError)
    {
        if (rcfError >= RcfError_User)
        {
            return "non-RCF error";
        }

        switch (rcfError)
        {
        case RcfError_Ok                            :   return "No error.";
        case RcfError_Unspecified                   :   return "Unknown error.";
        case RcfError_ServerMessageLength           :   return "Server side message length error.";
        case RcfError_ClientMessageLength           :   return "Client side message length error.";
        case RcfError_Serialization                 :   return "Data serialization error.";
        case RcfError_Deserialization               :   return "Data deserialization error.";
        case RcfError_UserModeException             :   return "Server side user exception.";
        case RcfError_UnknownEndpoint               :   return "Unknown endpoint.";
        case RcfError_EndpointPassword              :   return "Incorrect endpoint password.";
        case RcfError_EndpointDown                  :   return "Endpoint unavailable.";
        case RcfError_EndpointRetry                 :   return "Endpoint temporarily unavailable (try again).";
        case RcfError_ClientConnectTimeout          :   return "Client connect operation timed out (server not started?).";
        case RcfError_PeerDisconnect                :   return "Unexpected peer disconnection.";
        case RcfError_ClientCancel                  :   return "Remote call cancelled by client.";
        case RcfError_StubAssignment                :   return "Incompatible stub assignment.";
        case RcfError_PayloadFilterMismatch         :   return "Message filter mismatch.";
        case RcfError_OpenSslFilterInit             :   return "Failed to initialize OpenSSL filter.";
        case RcfError_OpenSslLoadCert               :   return "Failed to load OpenSSL certificate file.";
        case RcfError_UnknownPublisher              :   return "Unknown publisher name.";
        case RcfError_UnknownFilter                 :   return "Unknown filter type.";
        case RcfError_NoServerStub                  :   return "Server side object not found.";
        case RcfError_Sspi                          :   return "SSPI filter error.";
        case RcfError_SspiAuthFail                  :   return "SSPI authorization failed.";
        case RcfError_SspiInit                      :   return "Failed to initialize SSPI filter.";
        case RcfError_UnknownSubscriber             :   return "Unknown subscriber.";
        case RcfError_ClientReadTimeout             :   return "Client read operation timed out (no response from peer).";
        case RcfError_ClientReadFail                :   return "Client read operation failed.";
        case RcfError_ClientWriteTimeout            :   return "Client write operation timed out.";
        case RcfError_ClientWriteFail               :   return "Client write operation failed.";
        case RcfError_ClientConnectFail             :   return "Client connect operation failed.";
        case RcfError_Filter                        :   return "Filter error.";
        case RcfError_Socket                        :   return "Socket error.";
        case RcfError_FnId                          :   return "Invalid function id.";
        case RcfError_UnknownInterface              :   return "Unknown object interface.";
        case RcfError_NoEndpoint                    :   return "No endpoint.";
        case RcfError_TransportCreation             :   return "Failed to create transport.";
        case RcfError_FilterCount                   :   return "Invalid number of filters.";
        case RcfError_FilterMessage                 :   return "Failed to filter message.";
        case RcfError_UnfilterMessage               :   return "Failed to unfilter message.";
        case RcfError_SspiCredentials               :   return "SSPI credentials failure.";
        case RcfError_SspiEncrypt                   :   return "SSPI encryption failure.";
        case RcfError_SspiDecrypt                   :   return "SSPI decryption failure.";
        case RcfError_SspiImpersonation             :   return "SSPI impersonation failure.";
        case RcfError_NotConnected                  :   return "Send operation attempted without connecting.";
        case RcfError_SocketClose                   :   return "Failed to close socket.";
        case RcfError_SocketBind                    :   return "Failed to bind socket to port (port already in use?).";
        case RcfError_ZlibDeflate                   :   return "Zlib compression error.";
        case RcfError_ZlibInflate                   :   return "Zlib decompression error.";
        case RcfError_Zlib                          :   return "Zlib error.";
        case RcfError_UnknownSerializationProtocol  :   return "Unknown serialization protocol.";
        case RcfError_InvalidErrorMessage           :   return "Invalid error message from server.";
        case SfError_NoCtor                         :   return "Construction not supported for this type.";
        case SfError_RefMismatch                    :   return "Can't deserialize a reference into a non-reference object.";
        case SfError_DataFormat                     :   return "Input data format error.";
        case SfError_ReadFailure                    :   return "Failed to read data from underlying stream.";
        case SfError_WriteFailure                   :   return "Failed to write data to underlying stream.";
        case SfError_BaseDerivedRegistration        :   return "Base/derived pair not registered.";
        case SfError_TypeRegistration               :   return "Type not registered.";
        case RcfError_BadException                  :   return "Non std::exception-derived exception was thrown.";
        case RcfError_Decoding                      :   return "Decoding error.";
        case RcfError_Encoding                      :   return "Encoding error.";
        case RcfError_TokenRequestFailed            :   return "No tokens available.";
        case RcfError_ObjectFactoryNotFound         :   return "Object factory not found.";
        case RcfError_PortInUse                     :   return "Port already in use.";
        case RcfError_DynamicObjectNotFound         :   return "Server side object for given token not found.";
        case RcfError_VersionMismatch               :   return "Version mismatch.";
        case RcfError_RepeatedRetries               :   return "Too many client retries.";
        case RcfError_SslCertVerification           :   return "SSL certificate verification failure.";
        case RcfError_OutOfBoundsLength             :   return "Out of bounds length field.";
        case RcfError_FiltersLocked                 :   return "Filters locked.";
        case RcfError_Pipe                          :   return "Pipe error.";
        case RcfError_AnySerializerNotFound         :   return "boost::any serializer not registered for the given type.";
        case RcfError_ConnectionLimitExceeded       :   return "The server has reached its connection limit.";
        case RcfError_DeserializationNullPointer    :   return "Null pointer deserialization error.";
        case RcfError_PipeNameTooLong               :   return "Pipe name too long.";
        case RcfError_PingBack                      :   return "Received ping back message.";
        case RcfError_NoPingBackService             :   return "A ping back service has not been added to the server.";
        case RcfError_NoDownload                    :   return "The specified download does not exist.";
        case RcfError_FileOffset                    :   return "The specified file offset is invalid.";
        case RcfError_NoUpload                      :   return "The specified upload does not exist.";            
        case RcfError_FileOpen                      :   return "Failed to open file.";
        case RcfError_FileRead                      :   return "Failed to read from file.";
        case RcfError_FileWrite                     :   return "Failed to write to file.";
        case RcfError_UploadFailed                  :   return "Upload failed to complete.";
        case RcfError_UploadInProgress              :   return "Upload still in progress.";
        case RcfError_ConcurrentUpload              :   return "Cannot upload on several connections simultaneously.";
        case RcfError_UploadFileSize                :   return "File upload exceeding size limit.";
        case RcfError_AccessDenied                  :   return "Access denied.";
        case RcfError_PingBackTimeout               :   return "Failed to receive pingbacks from server.";
        case RcfError_AllThreadsBusy                :   return "All server threads are busy.";
        case RcfError_UnsupportedRuntimeVersion     :   return "Unsupported RCF runtime version.";
        case RcfError_FutureDereference             :   return "Cannot dereference un-signalled Future<>.";
        case RcfError_FdSetSize                     :   return "FD_SETSIZE limit exceeded.";
        case RcfError_DnsLookup                     :   return "DNS lookup of network address failed.";
        case RcfError_SspiHandshakeExtraData        :   return "SSPI handshake protocol error (extra data).";
        default                                     :   return "No available error message.";
        }
    }

    std::string getOsErrorString(int osError)
    {
        return Platform::OS::GetErrorString(osError);
    }

    std::string getSubSystemName(int subSystem)
    {
        switch (subSystem)
        {
        case RcfSubsystem_None                      :   return "No sub system.";
        case RcfSubsystem_Os                        :   return "Operating system";
        case RcfSubsystem_Zlib                      :   return "Zlib";
        case RcfSubsystem_OpenSsl                   :   return "OpenSSL";
        case RcfSubsystem_Asio                      :   return "Boost.Asio";
        default                                     :   return "No available sub system name.";
        }
    }

    // Exception

    Exception::Exception() :
        std::runtime_error(""),
        mWhat(),
        mError(RcfError_Ok),
        mSubSystemError(RCF_DEFAULT_INIT),
        mSubSystem(RCF_DEFAULT_INIT)
    {}

    Exception::Exception(
        const std::string &     what, 
        const std::string &     context) :
            std::runtime_error(""),
            mWhat(what),
            mContext(context),
            mError(RcfError_Unspecified),
            mSubSystemError(RCF_DEFAULT_INIT),
            mSubSystem(RCF_DEFAULT_INIT)
    {}

    Exception::Exception(
        int                     error,
        const std::string &     what,
        const std::string &     context) :
            std::runtime_error(""),
            mWhat(what),
            mContext(context),
            mError(error),
            mSubSystemError(RCF_DEFAULT_INIT),
            mSubSystem(RCF_DEFAULT_INIT)
    {}

    Exception::Exception(
        int                     error,
        int                     subSystemError,
        int                     subSystem,
        const std::string &     what,
        const std::string &     context) :
            std::runtime_error(""),
            mWhat(what),
            mContext(context),
            mError(error),
            mSubSystemError(subSystemError),
            mSubSystem(subSystem)
    {}

    Exception::~Exception() throw()
    {}

    bool Exception::good() const
    {
        return mError == RcfError_Ok;
    }

    bool Exception::bad() const
    {
        return !good();
    }

    const char *Exception::what() const throw()
    {
        mTranslatedWhat = translate();
        return mTranslatedWhat.c_str();
    }

    int Exception::getError() const
    {
        return mError;
    }

    int Exception::getSubSystemError() const
    {
        return mSubSystemError;
    }

    int Exception::getSubSystem() const
    {
        return mSubSystem;
    }

    void Exception::setContext(const std::string &context)
    {
        mContext = context;
    }

    std::string Exception::getContext() const
    {
        return mContext;
    }

    void Exception::setWhat(const std::string &what)
    {
        mWhat = what;
    }

    std::string Exception::getWhat() const
    {
        return mWhat;
    }

    std::string Exception::translate() const
    {
        std::string osErr;
        if (mSubSystem == RcfSubsystem_Os)
        {
            osErr = getOsErrorString(mSubSystemError);
        }

        std::ostringstream os;
        os
            << "[" << mError << ": " << getErrorString(mError) << "]"
            << "[" << mSubSystem << ": " << getSubSystemName(mSubSystem) << "]"
            << "[" << mSubSystemError << ": " << osErr << "]"
            << "[What: " << mWhat << "]"
            << "[Context: " << mContext << "]";
        return os.str();
    }

    void Exception::throwSelf() const
    {
        // If your code traps here, check that you have overridden throwSelf() in 
        // all classes derived from Exception.

        RCF_ASSERT(
            typeid(*this) == typeid(Exception))
            (typeid(*this));

        RCF_THROW((*this));
    }

    // RemoteException

    RemoteException::RemoteException()
    {}

    RemoteException::RemoteException(
        int                     remoteError,
        const std::string &     remoteWhat,
        const std::string &     remoteContext,
        const std::string &     remoteExceptionType) :
            Exception(
                remoteError,
                remoteWhat,
                remoteContext),
                mRemoteExceptionType(remoteExceptionType)
    {}

    RemoteException::RemoteException(
        int                     remoteError,
        int                     remoteSubSystemError,
        int                     remoteSubSystem,
        const std::string &     remoteWhat,
        const std::string &     remoteContext,
        const std::string &     remoteExceptionType) :
            Exception(
                remoteError,
                remoteSubSystemError,
                remoteSubSystem,
                remoteWhat,
                remoteContext),
                mRemoteExceptionType(remoteExceptionType)
    {}

    RemoteException::~RemoteException() throw()
    {}

    const char *RemoteException::what() const throw()
    {
        mTranslatedWhat = "[Remote]" + translate();
        return mTranslatedWhat.c_str();
    }

    std::string RemoteException::getRemoteExceptionType() const
    {
        return mRemoteExceptionType;
    }

    void RemoteException::throwSelf() const
    {
        // If your code traps here, check that you have overridden throwSelf() in 
        // all classes derived from RemoteException.

        RCF_ASSERT(
            typeid(*this) == typeid(RemoteException))
            (typeid(*this));

        RCF_THROW((*this));
    }

    // VersioningException

    VersioningException::VersioningException(int version) :
        RemoteException(RcfError_VersionMismatch),
        mVersion(version)
    {}

    VersioningException::~VersioningException() throw()
    {}

    int VersioningException::getVersion() const
    {
        return mVersion;
    }

#ifdef RCF_USE_SF_SERIALIZATION

    void RemoteException::serialize(SF::Archive &ar, const unsigned int)
    {
        ar
            & mWhat
            & mContext
            & mError
            & mSubSystemError
            & mSubSystem
            & mRemoteExceptionType;
    }

#endif

} // namespace RCF
