
//******************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005 - 2009, Jarl Lindrud. All rights reserved.
// Consult your license for conditions of use.
// Version: 1.1
// Contact: jarl.lindrud <at> gmail.com 
//******************************************************************************

#include <SF/Archive.hpp>

#include <SF/Stream.hpp>

namespace SF {


    Archive::Archive(Direction dir, I_Stream *stream) :
        mDir(dir),
        mStream(stream),
        mLabel(),
        mFlags(RCF_DEFAULT_INIT)
    {}

    Archive & Archive::operator&(Flag flag)
    {
        setFlag(flag);
        return *this;
    }

    bool Archive::isRead() const
    {
        return mDir == READ;
    }

    bool Archive::isWrite() const
    {
        return mDir == WRITE;
    }

    I_Stream *Archive::getStream() const
    {
        return mStream;
    }

    bool Archive::isFlagSet(Flag flag) const
    {
        return mFlags & flag ? true : false;
    }

    void Archive::setFlag(Flag flag, bool bEnable)
    {
        if (bEnable)
        {
            mFlags |= flag;
        }
        else
        {
            mFlags &= ~flag;
        }
    }

    void Archive::clearFlag(Flag flag)
    {
        setFlag(flag, false);
    }

    void Archive::clearState()
    {
        mLabel = "";
        mFlags = 0;
    }

    DataPtr & Archive::getLabel()
    {
        return mLabel;
    }

    bool Archive::verifyAgainstArchiveSize(std::size_t bytesToRead)
    {
        IStream * pis = dynamic_cast<IStream *>(mStream);
        return pis ?
            pis->verifyAgainstArchiveSize(bytesToRead) :
            false;
    }

} // namespace SF
