
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_RCF_EXCEPTION_HPP
#define INCLUDE_RCF_EXCEPTION_HPP

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

#include <RCF/util/DefaultInit.hpp>
#include <RCF/Export.hpp>
#include <RCF/SerializationDefs.hpp>
#include <RCF/Tools.hpp>
#include <RCF/TypeTraits.hpp>

#include <boost/version.hpp>
#if defined(RCF_USE_BOOST_SERIALIZATION) && BOOST_VERSION > 103301
#include <boost/serialization/nvp.hpp>
#endif

namespace SF {
    class Archive;
}

namespace RCF {

    // RCF error codes
    // range 0-1000 reserved for RCF, remaining range can be used independently of RCF

    static const int RcfError_Ok                                =  0;
    static const int RcfError_Unspecified                       =  1;
    static const int RcfError_ServerMessageLength               =  2;
    static const int RcfError_ClientMessageLength               =  3;
    static const int RcfError_Serialization                     =  4;
    static const int RcfError_Deserialization                   =  5;
    static const int RcfError_UserModeException                 =  6;
    static const int RcfError_UnknownEndpoint                   =  8;
    static const int RcfError_EndpointPassword                  =  9;
    static const int RcfError_EndpointDown                      = 10;
    static const int RcfError_EndpointRetry                     = 11;
    static const int RcfError_ClientConnectTimeout              = 16;
    static const int RcfError_PeerDisconnect                    = 17;
    static const int RcfError_ClientCancel                      = 18;
    static const int RcfError_StubAssignment                    = 19;
    static const int RcfError_PayloadFilterMismatch             = 20;
    static const int RcfError_OpenSslFilterInit                 = 21;
    static const int RcfError_OpenSslLoadCert                   = 22;
    static const int RcfError_UnknownPublisher                  = 23;
    static const int RcfError_UnknownFilter                     = 24;
    static const int RcfError_NoServerStub                      = 25;
    static const int RcfError_Sspi                              = 26;
    static const int RcfError_SspiAuthFail                      = 27;
    static const int RcfError_SspiInit                          = 28;
    static const int RcfError_UnknownSubscriber                 = 29;
    static const int RcfError_ClientReadTimeout                 = 30;
    static const int RcfError_ClientReadFail                    = 31;
    static const int RcfError_ClientWriteTimeout                = 32;
    static const int RcfError_ClientWriteFail                   = 33;
    static const int RcfError_ClientConnectFail                 = 34;
    static const int RcfError_Filter                            = 35;
    static const int RcfError_Socket                            = 36;
    static const int RcfError_FnId                              = 37;
    static const int RcfError_UnknownInterface                  = 38;
    static const int RcfError_NoEndpoint                        = 39;
    static const int RcfError_TransportCreation                 = 40;
    static const int RcfError_FilterCount                       = 41;
    static const int RcfError_FilterMessage                     = 42;
    static const int RcfError_UnfilterMessage                   = 43;
    static const int RcfError_SspiCredentials                   = 44;
    static const int RcfError_SspiEncrypt                       = 45;
    static const int RcfError_SspiDecrypt                       = 46;
    static const int RcfError_SspiImpersonation                 = 47;
    static const int RcfError_NotConnected                      = 48;
    static const int RcfError_SocketClose                       = 49;
    static const int RcfError_ZlibDeflate                       = 50;
    static const int RcfError_ZlibInflate                       = 51;
    static const int RcfError_Zlib                              = 52;
    static const int RcfError_UnknownSerializationProtocol      = 53;
    static const int RcfError_InvalidErrorMessage               = 54;
    static const int SfError_NoCtor                             = 55;
    static const int SfError_RefMismatch                        = 56;
    static const int SfError_DataFormat                         = 57;
    static const int SfError_ReadFailure                        = 58;
    static const int SfError_WriteFailure                       = 59;
    static const int SfError_BaseDerivedRegistration            = 60;
    static const int SfError_TypeRegistration                   = 61;
    static const int RcfError_BadException                      = 62;
    static const int RcfError_SocketBind                        = 63;
    static const int RcfError_Decoding                          = 64;
    static const int RcfError_Encoding                          = 65;
    static const int RcfError_TokenRequestFailed                = 66;
    static const int RcfError_ObjectFactoryNotFound             = 67;
    static const int RcfError_PortInUse                         = 68;
    static const int RcfError_DynamicObjectNotFound             = 69;
    static const int RcfError_VersionMismatch                   = 70;
    static const int RcfError_RepeatedRetries                   = 71;
    static const int RcfError_SslCertVerification               = 72;
    static const int RcfError_OutOfBoundsLength                 = 73;
    static const int RcfError_FiltersLocked                     = 74;
    static const int RcfError_Pipe                              = 75;
    static const int RcfError_AnySerializerNotFound             = 76;
    static const int RcfError_ConnectionLimitExceeded           = 77;
    static const int RcfError_DeserializationNullPointer        = 78;
    static const int RcfError_PipeNameTooLong                   = 79;
    static const int RcfError_PingBack                          = 80;
    static const int RcfError_NoPingBackService                 = 81;
    static const int RcfError_NoDownload                        = 82;
    static const int RcfError_FileOffset                        = 83;
    static const int RcfError_NoUpload                          = 84;
    static const int RcfError_FileOpen                          = 85;
    static const int RcfError_FileRead                          = 86;
    static const int RcfError_FileWrite                         = 87;
    static const int RcfError_UploadFailed                      = 88;
    static const int RcfError_UploadInProgress                  = 89;
    static const int RcfError_ConcurrentUpload                  = 90;
    static const int RcfError_UploadFileSize                    = 91;
    static const int RcfError_AccessDenied                      = 92;
    static const int RcfError_PingBackTimeout                   = 93;
    static const int RcfError_AllThreadsBusy                    = 94;
    static const int RcfError_UnsupportedRuntimeVersion         = 95;
    static const int RcfError_FutureDereference                 = 96;
    static const int RcfError_FdSetSize                         = 97;
    static const int RcfError_DnsLookup                         = 98;
    static const int RcfError_SspiHandshakeExtraData            = 99;
    static const int RcfError_User                              = 1001;

    // RCF subsystem identifiers
    static const int RcfSubsystem_None                          = 0;
    static const int RcfSubsystem_Os                            = 1;
    static const int RcfSubsystem_Zlib                          = 2;
    static const int RcfSubsystem_OpenSsl                       = 3;
    static const int RcfSubsystem_Asio                          = 4;

    RCF_EXPORT std::string getErrorString(int rcfError);
    RCF_EXPORT std::string getSubSystemName(int subSystem);
    RCF_EXPORT std::string getOsErrorString(int osError);

    /// Base class of all exceptions thrown by RCF.
    class RCF_EXPORT Exception : public std::runtime_error
    {
    public:
        Exception();

        Exception(
            const std::string &     what, 
            const std::string &     context = "");

        Exception(
            int error,
            const std::string &     what = "",
            const std::string &     context = "");

        Exception(
            int                     error,
            int                     subSystemError,
            int                     subSystem = RcfSubsystem_Os,
            const std::string &     what = "",
            const std::string &     context = "");

        ~Exception() throw();

        virtual std::auto_ptr<Exception> clone() const
        {
            return std::auto_ptr<Exception>(
                new Exception(*this));
        }

        bool good() const;
        bool bad() const;

        const char *    what()                  const throw();
        int             getError()              const;
        int             getSubSystemError()     const;
        int             getSubSystem()          const;
        std::string     getContext()            const;
        std::string     getWhat()               const;

        void            setContext(const std::string &context);
        void            setWhat(const std::string &what);

        virtual void    throwSelf() const;

    protected:

        std::string     translate()             const;

        // protected to make serialization of RemoteException simpler
    protected:

        std::string             mWhat;
        std::string             mContext;
        int                     mError;
        int                     mSubSystemError;
        int                     mSubSystem;

        mutable std::string     mTranslatedWhat;
    };

    typedef boost::shared_ptr<Exception> ExceptionPtr;

    class RCF_EXPORT RemoteException : public Exception
    {
    public:
        RemoteException();

        RemoteException(
            int                     remoteError,
            const std::string &     remoteWhat = "",
            const std::string &     remoteContext = "",
            const std::string &     remoteExceptionType = "");

        RemoteException(
            int                     remoteError,
            int                     remoteSubSystemError,
            int                     remoteSubSystem,
            const std::string &     remoteWhat = "",
            const std::string &     remoteContext = "",
            const std::string &     remoteExceptionType = "");

        ~RemoteException() throw();

        const char *what() const throw();

        std::string getRemoteExceptionType() const;

#ifdef RCF_USE_SF_SERIALIZATION

        void serialize(SF::Archive &ar, const unsigned int);

#endif

#ifdef RCF_USE_BOOST_SERIALIZATION

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar
                & boost::serialization::make_nvp("What", mWhat)
                & boost::serialization::make_nvp("Context", mContext)
                & boost::serialization::make_nvp("Error", mError)
                & boost::serialization::make_nvp("Subsystem Error", mSubSystemError)
                & boost::serialization::make_nvp("Subsystem", mSubSystem)
                & boost::serialization::make_nvp("Remote Exception Type", mRemoteExceptionType);
        }

#endif

        std::auto_ptr<Exception> clone() const
        {
            return std::auto_ptr<Exception>(
                new RemoteException(*this));
        }

        void throwSelf() const;

    private:
        std::string mRemoteExceptionType;
    };

#define RCF_DEFINE_EXCEPTION(E, PE)                             \
    class E : public PE                                         \
    {                                                           \
    public:                                                     \
        E(                                                      \
            const std::string &what = "") :                     \
                PE(RcfError_Unspecified, what)                  \
        {}                                                      \
        E(                                                      \
            int error,                                          \
            const std::string &what = "") :                     \
                PE(error, what)                                 \
        {}                                                      \
        E(                                                      \
            int error,                                          \
            int subSystemError,                                 \
            int subSystem,                                      \
            const std::string &what = "") :                     \
                PE(error, subSystemError, subSystem, what)      \
        {}                                                      \
        std::auto_ptr<Exception> clone() const                  \
        {                                                       \
            return std::auto_ptr<Exception>(                    \
                new E(*this));                                  \
        }                                                       \
        void throwSelf() const                                  \
        {                                                       \
            RCF_THROW((*this));                                 \
        }                                                       \
        ~E() throw()                                            \
        {}                                                      \
    };

    RCF_DEFINE_EXCEPTION(SerializationException,        Exception)
    RCF_DEFINE_EXCEPTION(AssertionFailureException,     Exception)
    RCF_DEFINE_EXCEPTION(FilterException,               Exception)

    class RCF_EXPORT VersioningException : public RemoteException
    {
    public:
        VersioningException(int version);
        ~VersioningException() throw();
        int getVersion() const;

        std::auto_ptr<Exception> clone() const
        {
            return std::auto_ptr<Exception>(
                new VersioningException(*this));
        }

        void throwSelf() const
        {
            RCF_THROW((*this));
        }

    private:
        int mVersion;
    };

#undef RCF_DEFINE_EXCEPTION

} // namespace RCF

#include <memory>
#include <boost/type_traits.hpp>
RCF_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION( RCF::RemoteException )
RCF_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION( std::auto_ptr<RCF::RemoteException> )

#endif // ! INCLUDE_RCF_EXCEPTION_HPP
