
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_RCF_FILETRANSFERSERVICE_HPP
#define INCLUDE_RCF_FILETRANSFERSERVICE_HPP

#include <fstream>
#include <map>

#include <RCF/ServerInterfaces.hpp>
#include <RCF/Service.hpp>
#include <RCF/StubEntry.hpp>
#include <RCF/Token.hpp>

namespace RCF {

    class FileUploadInfo;
    class FileDownloadInfo;

    typedef boost::shared_ptr<FileUploadInfo>                   FileUploadInfoPtr;
    typedef boost::shared_ptr<FileDownloadInfo>                 FileDownloadInfoPtr;

    typedef boost::function1<bool, const FileUploadInfo &>      UploadAccessCallback;
    typedef boost::function1<bool, const FileDownloadInfo &>    DownloadAccessCallback;

    class FileUploadInfo : 
        public TokenMapped, 
        boost::noncopyable
    {
    public:
        FileUploadInfo() : 
            mFileSize(RCF_DEFAULT_INIT), 
            mCompleted(RCF_DEFAULT_INIT),
            mFailed(RCF_DEFAULT_INIT),
            mTimeStampMs(RCF_DEFAULT_INIT)
        {}

        ~FileUploadInfo();

        std::string             mUploadPath;
        std::ofstream           mFileStream;
        boost::uint64_t         mFileSize;
        bool                    mCompleted;
        bool                    mFailed;
        boost::uint32_t         mTimeStampMs;

        // Only one uploader allowed at a time.
        Mutex                   mUploaderMutex;
        RcfSessionWeakPtr       mUploader;        
    };

    class FileDownloadInfo : 
        public TokenMapped, 
        boost::noncopyable
    {
    public:

        FileDownloadInfo() :
            mFileSize(RCF_DEFAULT_INIT)
        {}

        ~FileDownloadInfo();
        
        std::string             mDownloadPath;
        boost::uint64_t         mFileSize;

        Mutex                   mFileStreamMutex;
        std::ifstream           mFileStream;

        // Or some other criteria.
        DownloadAccessCallback  mAccessCallback;
    };

    typedef boost::shared_ptr<FileUploadInfo>   FileUploadInfoPtr;
    typedef boost::shared_ptr<FileDownloadInfo> FileDownloadInfoPtr;

    class RCF_EXPORT FileTransferService : public I_Service
    {
    public:
        FileTransferService(const std::string & tempFileDirectory = "");

        void setUploadCallback(UploadAccessCallback uploadCallback);

        //----------------------------------------------------------------------
        // Uploads

        // Fails if transfer is not complete.
        boost::int32_t      findUploadedFile(
                                Token               token, 
                                std::string &       path);

        boost::int32_t      findUploadedFile(
                                std::string &       path);

        void                removeUpload(
                                Token               token);

        void                removeUpload();


        //----------------------------------------------------------------------
        // Downloads

        // Fails if file does not exist or there are no more tokens.
        boost::int32_t      prepareDownload(
                                const std::string & whichFile,
                                Token & token);

        boost::int32_t      prepareDownload(
                                const std::string & whichFile);

        // Always succeeds.
        void                removeDownload(
                                Token token);

        void                removeDownload();

        

        //----------------------------------------------------------------------
        // Remotely accessible.

        boost::uint32_t     uploadChunk(
                                boost::uint64_t     pos,
                                ByteBuffer          byteBuffer, 
                                boost::uint64_t     fileSize);

        boost::uint32_t     uploadChunk(
                                Token &             token, 
                                boost::uint64_t     pos,
                                ByteBuffer          byteBuffer, 
                                boost::uint64_t     fileSize);

        boost::uint32_t     downloadChunk(
                                boost::uint64_t     pos, 
                                boost::uint32_t     chunkSize, 
                                ByteBuffer &        byteBuffer,
                                boost::uint64_t &   fileSize);

        boost::uint32_t     downloadChunk(
                                Token               token, 
                                boost::uint64_t     pos, 
                                boost::uint32_t     chunkSize, 
                                ByteBuffer &        byteBuffer,
                                boost::uint64_t &   fileSize);        

        //----------------------------------------------------------------------

    private:

        boost::int32_t      prepareDownloadImpl(
                                const std::string & whichFile,
                                Token *             token);

        boost::int32_t      prepareUploadImpl(
                                boost::uint64_t     fileSize, 
                                Token *             token);

        FileUploadInfoPtr   findTokenMappedUpload(
                                const Token &       token);

        FileDownloadInfoPtr findTokenMappedDownload(
                                const Token &       token);

        boost::uint32_t     uploadChunkImpl(
                                Token *             pToken, 
                                boost::uint64_t     pos,
                                ByteBuffer          byteBuffer, 
                                boost::uint64_t     fileSize);

        boost::uint32_t     downloadChunkImpl(
                                Token *             pToken, 
                                boost::uint64_t     offset, 
                                boost::uint32_t     chunkSize, 
                                ByteBuffer &        byteBuffer,
                                boost::uint64_t &   fileSize);

        void                onServiceAdded(
                                RcfServer &         server);

        void                onServiceRemoved(
                                RcfServer &         server);

        std::string             mTempFileDirectory;

        UploadAccessCallback    mUploadCallback;
    };

    typedef boost::shared_ptr<FileTransferService> FileTransferServicePtr;

    RCF_EXPORT bool deleteFile(const std::string & path);
    RCF_EXPORT bool fileExists(const std::string & path);

} // namespace RCF

#endif // ! INCLUDE_RCF_FILETRANSFERSERVICE_HPP
