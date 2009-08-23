
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#ifndef INCLUDE_SF_STREAM_HPP
#define INCLUDE_SF_STREAM_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <strstream>

#include <boost/noncopyable.hpp>

#include <RCF/Export.hpp>

#include <SF/Archive.hpp>
#include <SF/DataPtr.hpp>
#include <SF/Encoding.hpp>
#include <SF/I_Stream.hpp>

#include <SF/Serializer.hpp>

namespace SF {

    //**************************************************
    // Encoding of object data

    template<typename E>
    class Encoding : public I_Encoding
    {
    public:
        virtual UInt32 getCount(DataPtr &data, const std::type_info &type)
        {
            return countElements( (E *) 0, data, type);
        }

        virtual void toData(DataPtr &data, void *pvObject, const std::type_info &type, int nCount)
        {
            encodeElements( (E *) 0, data, pvObject, type, nCount);
        }

        virtual void toObject(DataPtr &data, void *pvObject, const std::type_info &type, int nCount)
        {
            decodeElements( (E *) 0, data, pvObject, type, nCount);
        }
    };

    template<typename E>
    class WithEncoding : public I_WithEncoding
    {
    public:
        virtual I_Encoding &getEncoding() { return e; }
    private:
        E e;
    };

    //**************************************************
    // Context handling

    // Context

    class RCF_EXPORT ContextRead : public I_ContextRead
    {
    public:
        ContextRead();
        virtual ~ContextRead();
        virtual void add(SF::UInt32 nid, const ObjectId &id);
        virtual void add(void *ptr, const std::type_info &objType, void *pObj);
        virtual bool query(SF::UInt32 nid, ObjectId &id);
        virtual bool query(void *ptr, const std::type_info &objType, void *&pObj);
        virtual void clear();
    private:
        bool bEnabled_;
        std::auto_ptr<std::map<UInt32, ObjectId> > nid_id_ptr_;
        std::auto_ptr<std::map<std::string, std::map< void *, void * > > > type_ptr_obj_ptr_;
    };

    class RCF_EXPORT ContextWrite : public I_ContextWrite
    {
    public:
        ContextWrite();
        virtual ~ContextWrite();
        virtual void setEnabled(bool enabled);
        virtual bool getEnabled();
        virtual void add(const ObjectId &id, UInt32 &nid);
        virtual bool query(const ObjectId &id, UInt32 &nid);
        virtual void clear();
    private:
        bool bEnabled_;
        UInt32 currentId_;
        std::auto_ptr<std::map<ObjectId, UInt32> > id_nid_ptr_;
    };

    class RCF_EXPORT WithContextRead : public I_WithContextRead
    {
    public:
        virtual ~WithContextRead();
        virtual I_ContextRead &getContext();
    private:
        ContextRead context;
    };

    class RCF_EXPORT WithContextWrite : public I_WithContextWrite
    {
    public:
        virtual ~WithContextWrite();
        virtual I_ContextWrite &getContext();
        void enableContext() { getContext().setEnabled(true); }
        void disableContext() { getContext().setEnabled(false); }
        void clearContext() { getContext().clear(); }
    private:
        ContextWrite context;
    };

    //**************************************************
    // Stream local storage

    class RCF_EXPORT LocalStorage : public I_LocalStorage, boost::noncopyable
    {
    public:
        LocalStorage();
        virtual ~LocalStorage();
        void setNode(Node *);
        Node *getNode();

    private:
        Node *pNode_;
    };

    class RCF_EXPORT WithLocalStorage : public I_WithLocalStorage
    {
    public:
        virtual ~WithLocalStorage();
        virtual I_LocalStorage &getLocalStorage();
    private:
        LocalStorage localStorage_;
    };

    //****************************************************

    class WithSemanticsRead : public I_Stream
    {
    public:
        virtual ~WithSemanticsRead() {}

        // input dispatching

        template<typename T>
        WithSemanticsRead &operator>>(T &t)
        {
            Archive ar(Archive::READ, static_cast<I_Stream *>(this) );
            ar & t;
            return *this;
        }

        template<typename T>
        WithSemanticsRead &operator>>(const T &t)
        {
            Archive ar(Archive::READ, static_cast<I_Stream *>(this) );
            ar & t;
            return *this;
        }

    };

    class WithSemanticsWrite : public I_Stream
    {
    public:
        virtual ~WithSemanticsWrite() {}

        template<typename T>
        WithSemanticsWrite &operator<<(const T &t)
        {
            Archive ar(Archive::WRITE, static_cast<I_Stream *>(this) );
            ar & t;
            return *this;
        }

    };


    // stream base classes

    typedef Encoding<Text> EncodingText;
    typedef Encoding<BinaryNative> EncodingBinaryNative;
    typedef Encoding<BinaryPortable> EncodingBinaryPortable;

    typedef WithEncoding<EncodingText> WithEncodingText;
    typedef WithEncoding<EncodingBinaryNative> WithEncodingBinaryNative;
    typedef WithEncoding<EncodingBinaryPortable> WithEncodingBinaryPortable;

    class Node;

    class RCF_EXPORT IStream :
        public WithSemanticsRead,
        public WithContextRead,
        public WithFormatRead,
        public WithLocalStorage,
        boost::noncopyable
    {
    public:
        IStream();
        IStream(std::istream &is, std::size_t archiveSize = 0);
        void setIs(std::istream &is, std::size_t archiveSize = 0);
        void clearState();

        bool supportsReadRaw();
        UInt32 readRaw(Byte8 *&pBytes, UInt32 nLength);
        UInt32 read(Byte8 *pBytes, UInt32 nLength);

        bool verifyAgainstArchiveSize(std::size_t bytesToRead);

    private:
        bool begin(Node &node);
        bool get(DataPtr &value);
        void end();
        UInt32 read_int(UInt32 &n);
        UInt32 read_byte(Byte8 &byte);
        void putback_byte(Byte8 byte);

        std::istream *      mpIs;
        std::istrstream *   mpIstr;
        std::size_t         mArchiveSize;
    };

    class RCF_EXPORT OStream :
        public WithSemanticsWrite,
        public WithContextWrite,
        public WithFormatWrite,
        public WithLocalStorage,
        boost::noncopyable
    {
    public:
        OStream();
        OStream(std::ostream &os);
        void setOs(std::ostream &os);
        void clearState();

        UInt32 writeRaw(const Byte8 *pBytes, UInt32 nLength);

    private:
        void begin(const Node &node);
        void put(const DataPtr &value);
        void end();
        UInt32 write_int(UInt32 n);
        UInt32 write_byte(Byte8 byte);
        UInt32 write(const Byte8 *pBytes, UInt32 nLength);

        std::ostream *mpOs;
    };

} // namespace SF


#endif // !INCLUDE_SF_STREAM_HPP
