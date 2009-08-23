
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <RCF/FileTransferService.hpp>

#include <boost/function.hpp>

#include <cstdio>
#include <iomanip>
#include <boost/limits.hpp>

#include <sys/stat.h>

#include <RCF/ObjectFactoryService.hpp>
#include <RCF/RcfServer.hpp>
#include <RCF/util/Platform/OS/Sleep.hpp>

namespace RCF {

    bool deleteFile(const std::string & path)
    {

#if defined(_MSC_VER) && _MSC_VER < 1310
        int ret = remove(path.c_str());
#else
        int ret = std::remove(path.c_str());
#endif

        int err = Platform::OS::BsdSockets::GetLastError();

        RCF_UNUSED_VARIABLE(err);
        return ret == 0;
    }

    bool fileExists(const std::string & path)
    {
        struct stat st = {0};
        int ret = stat(path.c_str(), &st);
        return ret == 0;
    }

    FileUploadInfo::~FileUploadInfo()
    {
        mFileStream.close();

        // Best effort only.
        deleteFile(mUploadPath);
    }

    FileDownloadInfo::~FileDownloadInfo()
    {
    }

    FileTransferService::FileTransferService(
        const std::string & tempFileDirectory) :
            mTempFileDirectory(tempFileDirectory)
    {
    }

    boost::int32_t FileTransferService::findUploadedFile(
        std::string & path)
    {
        return findUploadedFile(Token(), path);
    }

    FileUploadInfoPtr FileTransferService::findTokenMappedUpload(const Token & token)
    {
        FileUploadInfoPtr uploadInfoPtr;

        RCF::ObjectFactoryServicePtr ofsPtr = 
            RCF::getCurrentRcfSession().getRcfServer()
                .getObjectFactoryServicePtr();

        RCF_VERIFY(ofsPtr, Exception(RcfError_ObjectFactoryNotFound));

        TokenMappedPtr tokenMappedPtr = ofsPtr->getTokenMappedPtr(token);

        uploadInfoPtr = 
            boost::dynamic_pointer_cast<FileUploadInfo>(tokenMappedPtr);

        if (uploadInfoPtr)
        {
            uploadInfoPtr->touch();
        }

        return uploadInfoPtr;
    }

    FileDownloadInfoPtr FileTransferService::findTokenMappedDownload(
        const Token & token)
    {
        FileDownloadInfoPtr downloadInfoPtr;

        RCF::ObjectFactoryServicePtr ofsPtr = 
            RCF::getCurrentRcfSession().getRcfServer()
                .getObjectFactoryServicePtr();

        RCF_VERIFY(ofsPtr, Exception(RcfError_ObjectFactoryNotFound));

        TokenMappedPtr tokenMappedPtr = ofsPtr->getTokenMappedPtr(token);

        downloadInfoPtr = 
            boost::dynamic_pointer_cast<FileDownloadInfo>(tokenMappedPtr);

        if (downloadInfoPtr)
        {
            downloadInfoPtr->touch();
        }

        return downloadInfoPtr;
    }

    // TODO: FileTransferService could maintain an ObjectFactoryService pointer,
    // instead of looking it up through getCurrentRcfSession() each time.
    // ...

    boost::int32_t FileTransferService::findUploadedFile(
        Token token, 
        std::string & path)
    {
        FileUploadInfoPtr uploadInfoPtr;

        if (token == Token())
        {
            uploadInfoPtr = getCurrentRcfSession().mUploadInfoPtr;
        }
        else
        {
            uploadInfoPtr = findTokenMappedUpload(token);
        }

        if (!uploadInfoPtr)
        {
            return RcfError_NoUpload;
        }
        else if (uploadInfoPtr->mFailed)
        {
            return RcfError_UploadFailed;
        }
        else if (uploadInfoPtr->mCompleted)
        {
            path = uploadInfoPtr->mUploadPath;
            return RcfError_Ok;
        }
        else
        {
            return RcfError_UploadInProgress;
        }
    }

    boost::int32_t FileTransferService::prepareDownload(
        const std::string & whichFile)
    {
        return prepareDownloadImpl(whichFile, NULL);
    }

    boost::int32_t FileTransferService::prepareDownload(
        const std::string & whichFile,
        Token & token)
    {
        return prepareDownloadImpl(whichFile, &token);

    }

    boost::int32_t FileTransferService::prepareDownloadImpl(
        const std::string & whichFile,
        Token * pToken)
    {
        FileDownloadInfoPtr downloadInfoPtr( new FileDownloadInfo() );

        downloadInfoPtr->mDownloadPath = whichFile;

        downloadInfoPtr->mFileStream.open(
            whichFile.c_str(), 
            std::ios::binary);

        // Check that the stream opened OK.
        if (downloadInfoPtr->mFileStream)
        {
            downloadInfoPtr->mFileSize = fileSize(whichFile);

            if (pToken)
            {
                Token token;

                RCF::ObjectFactoryServicePtr ofsPtr = 
                    RCF::getCurrentRcfSession().getRcfServer()
                    .getObjectFactoryServicePtr();

                if (!ofsPtr)
                {
                    return RcfError_ObjectFactoryNotFound;
                }

                boost::int32_t ret = ofsPtr->addObject(downloadInfoPtr, token);
                
                if (ret != RcfError_Ok)
                {
                    return ret;
                }

                *pToken = token;
            }
            else
            {
                getCurrentRcfSession().mDownloadInfoPtr = downloadInfoPtr;
            }            

            return RcfError_Ok;
        }

        return RcfError_FileOpen;
    }

    boost::int32_t FileTransferService::prepareUploadImpl(
        boost::uint64_t fileSize,
        Token * pToken)
    {
        FileUploadInfoPtr uploadInfoPtr( new FileUploadInfo() );

        uploadInfoPtr->mFileSize = fileSize;

        uploadInfoPtr->mTimeStampMs = Platform::OS::getCurrentTimeMs();

        // Check the access callback (if any).
        if (    !mUploadCallback.empty()
            &&  !mUploadCallback(*uploadInfoPtr))
        {
            return RcfError_AccessDenied;
        }

        // Create a temp file to upload to.
        std::size_t tries = 0;
        while (tries++ < 10)
        {
            std::ostringstream os;
            os 
                << mTempFileDirectory
                << "RCF-Upload-" 
                << std::setw(10) // should be 10...
                << std::setfill('0')
                << rand();

            std::string tempFile = os.str();

            if (!fileExists(tempFile))
            {
                uploadInfoPtr->mFileStream.open( 
                    tempFile.c_str(), 
                    std::ios::binary | std::ios::trunc);

                if (uploadInfoPtr->mFileStream)
                {
                    uploadInfoPtr->mUploadPath = tempFile;
                    break;
                }
            }
        }

        // Check that stream opened OK.
        if (uploadInfoPtr->mFileStream)
        {
            if (pToken)
            {
                Token token;
                
                RCF::ObjectFactoryServicePtr ofsPtr = 
                    RCF::getCurrentRcfSession().getRcfServer()
                        .getObjectFactoryServicePtr();

                if (!ofsPtr)
                {
                    return RcfError_ObjectFactoryNotFound;
                }

                boost::int32_t ret = ofsPtr->addObject(uploadInfoPtr, token);

                if (ret != RcfError_Ok)
                {
                    return ret;
                }
                *pToken = token;
            }
            else
            {
                getCurrentRcfSession().mUploadInfoPtr = uploadInfoPtr;
            }
            
            return RcfError_Ok;
        }
        
        return RcfError_FileOpen;
    }

    void FileTransferService::removeUpload(Token token)
    {
        RCF::ObjectFactoryServicePtr ofsPtr = 
            RCF::getCurrentRcfSession().getRcfServer()
                .getObjectFactoryServicePtr();

        if (ofsPtr)
        {
            ofsPtr->deleteObject(token);
        }
    }

    void FileTransferService::removeDownload(Token token)
    {
        RCF::ObjectFactoryServicePtr ofsPtr = 
            RCF::getCurrentRcfSession().getRcfServer()
                .getObjectFactoryServicePtr();

        if (ofsPtr)
        {
            ofsPtr->deleteObject(token);
        }
    }

    void FileTransferService::removeUpload()
    {
        getCurrentRcfSession().mUploadInfoPtr.reset();
    }

    void FileTransferService::removeDownload()
    {
        getCurrentRcfSession().mDownloadInfoPtr.reset();
    }

    // Remotely accessible
    boost::uint32_t FileTransferService::uploadChunk(
        boost::uint64_t     offset,
        ByteBuffer          byteBuffer, 
        boost::uint64_t     fileSize)
    {
        return uploadChunkImpl(NULL, offset, byteBuffer, fileSize);
    }

    // Remotely accessible
    boost::uint32_t FileTransferService::uploadChunk(
        Token &             token, 
        boost::uint64_t     offset,
        ByteBuffer          byteBuffer, 
        boost::uint64_t     fileSize)
    {
        return uploadChunkImpl(&token, offset, byteBuffer, fileSize);
    }

    boost::uint32_t FileTransferService::uploadChunkImpl(
        Token *             pToken, 
        boost::uint64_t     offset,
        ByteBuffer          byteBuffer, 
        boost::uint64_t     fileSize)
    {
        // Find the upload.
        FileUploadInfoPtr uploadInfoPtr;

        // If this is the first chunk, create a new upload.
        if (offset == 0)
        {
            boost::uint32_t ret = prepareUploadImpl(fileSize, pToken);
            if (static_cast<int>(ret) != RcfError_Ok)
            {
                return ret;
            }
        }

        if (pToken == NULL)
        {
            uploadInfoPtr = getCurrentRcfSession().mUploadInfoPtr;
        }
        else
        {
            uploadInfoPtr = findTokenMappedUpload(*pToken);
        }
        
        if (!uploadInfoPtr)
        {
            return RcfError_NoUpload;
        }

        if (uploadInfoPtr->mFileSize != fileSize)
        {
            return RcfError_UploadFileSize;
        }

        // Check that we are on an appropriate connection.
        RcfSessionWeakPtr sessionWeakPtr = RCF::getCurrentRcfSessionPtr();
        {
            Lock lock(uploadInfoPtr->mUploaderMutex);
            if (uploadInfoPtr->mUploader == RcfSessionWeakPtr())
            {
                uploadInfoPtr->mUploader = sessionWeakPtr;
            }

            if (uploadInfoPtr->mUploader != sessionWeakPtr)
            {
                return RcfError_ConcurrentUpload;
            }
        }        
        
        std::ofstream & fout = uploadInfoPtr->mFileStream;

        // Check stream state.
        if (!fout)
        {
            return RcfError_FileWrite;
        }

        // Check the offset position.
        boost::uint64_t streamPos = fout.tellp();
        if (offset != streamPos || uploadInfoPtr->mCompleted)
        {
            return RcfError_FileOffset;
        }

        // Check the chunk size.
        if (streamPos + byteBuffer.getLength() > uploadInfoPtr->mFileSize)
        {
            return RcfError_UploadFileSize;
        }

        fout.write( byteBuffer.getPtr(), byteBuffer.getLength() );

        // Check write operation.
        if (fout.fail())
        {
            fout.close();
            uploadInfoPtr->mFailed = true;
            return RcfError_FileWrite;
        }

        uploadInfoPtr->mTimeStampMs = Platform::OS::getCurrentTimeMs();

        // Check if last chunk.
        streamPos = fout.tellp();
        if (streamPos == uploadInfoPtr->mFileSize)
        {
            fout.close();
            uploadInfoPtr->mCompleted = true;
        }

        return RcfError_Ok;
    }

    // Remotely accessible
    boost::uint32_t FileTransferService::downloadChunk(
        boost::uint64_t     offset, 
        boost::uint32_t     chunkSize, 
        ByteBuffer &        byteBuffer,
        boost::uint64_t &   fileSize)
    {
        return downloadChunkImpl(NULL, offset, chunkSize, byteBuffer, fileSize);
    }

    // Remotely accessible
    boost::uint32_t FileTransferService::downloadChunk(
        Token               token, 
        boost::uint64_t     offset, 
        boost::uint32_t     chunkSize, 
        ByteBuffer &        byteBuffer,
        boost::uint64_t &   fileSize)
    {
        return downloadChunkImpl(&token, offset, chunkSize, byteBuffer, fileSize);
    }
    
    boost::uint32_t FileTransferService::downloadChunkImpl(
        Token *             pToken, 
        boost::uint64_t     offset, 
        boost::uint32_t     chunkSize, 
        ByteBuffer &        byteBuffer,
        boost::uint64_t &   fileSize)
    {

        // Find the download.
        FileDownloadInfoPtr downloadInfoPtr;

        if (pToken == NULL)
        {
            downloadInfoPtr = getCurrentRcfSession().mDownloadInfoPtr;
        }
        else
        {
            downloadInfoPtr = findTokenMappedDownload(*pToken);
        }

        if (!downloadInfoPtr)
        {
            return RcfError_NoDownload;
        }

        // Check the access callback, if any.
        if (    !downloadInfoPtr->mAccessCallback.empty() 
            &&  !downloadInfoPtr->mAccessCallback(*downloadInfoPtr))
        {
            return RcfError_AccessDenied;
        }

        Lock lock(downloadInfoPtr->mFileStreamMutex);

        // Check offset.
        fileSize = downloadInfoPtr->mFileSize;
        if (offset >= fileSize)
        {
            return RcfError_FileOffset;
        }

        ByteBuffer temp(chunkSize);

        std::ifstream & fin = downloadInfoPtr->mFileStream;

#if defined(__GNUC__) && __GNUC__ == 2
        typedef std::streamoff          StreamOffset;
#else
        typedef std::istream::off_type  StreamOffset;
#endif

        const boost::uint64_t MaxOffset = 
            (std::numeric_limits<StreamOffset>::max)();

        bool needMultipleSeeks = false;

        if (sizeof(StreamOffset) < sizeof(boost::uint64_t))
        {
            if (offset > MaxOffset)
            {
                // Probably we are using a 32 bit STL and trying to seek to a
                // 64 bit offset.

                needMultipleSeeks = true;
            }
        }

        if (needMultipleSeeks)
        {
            const boost::uint64_t Step = MaxOffset;
            const boost::uint64_t Steps = offset / Step;
            const boost::uint64_t Remainder = offset - Steps*Step;

            fin.seekg( 
                0, 
                std::ios::beg);

            for (boost::uint64_t i=0; i<Steps; ++i)
            {
                fin.seekg( 
                    static_cast<StreamOffset>(MaxOffset),
                    std::ios::cur);
            }

            fin.seekg(
                static_cast<StreamOffset>(Remainder), 
                std::ios::cur);
        }
        else
        {
            fin.seekg( 
                static_cast<StreamOffset>(offset), 
                std::ios::beg);
        }
        

        std::size_t bytesRead = fin.read( 
            temp.getPtr(), 
            temp.getLength() ).gcount();

        // Check read operation.
        if (fin.fail() && !fin.eof())
        {
            fin.clear();
            fin.seekg(0, std::ios::beg);
            return RcfError_FileRead;
        }

        byteBuffer = ByteBuffer(temp, 0, bytesRead);

        // Check for EOF.
        RCF_ASSERT(offset + bytesRead <= downloadInfoPtr->mFileSize);
        if (fin.eof())
        {
            // Rewind back to the beginning of the file.
            fin.clear();
            fin.seekg(0, std::ios::beg);
        }

        return RcfError_Ok;        
    }

    void FileTransferService::setUploadCallback(UploadAccessCallback uploadCallback)
    {
        mUploadCallback = uploadCallback;
    }

    void FileTransferService::onServiceAdded(RcfServer &server)
    {
        server.bind( (I_FileTransferService *) NULL, *this);
    }

    void FileTransferService::onServiceRemoved(RcfServer &server)
    {
        server.unbind( (I_FileTransferService *) NULL);
    }

} // namespace RCF
