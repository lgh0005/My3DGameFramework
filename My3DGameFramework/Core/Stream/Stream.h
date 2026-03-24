#pragma once
#include "Pointer/RefCount.h"

namespace MGF3D
{
    MGF_CLASS_PTR(Stream)
    class Stream : public RefCount
    {
    public:
        virtual ~Stream() override = default;

        virtual bool   CanRead() const = 0;
        virtual bool   CanWrite() const = 0;
        virtual bool   CanSeek() const = 0;

        virtual uint64 GetLength() const = 0;
        virtual uint64 GetPosition() const = 0;
        virtual void   SetPosition(uint64 pos) = 0;

        virtual usize  Read(void* buffer, usize size) = 0;
        virtual usize  Write(const void* buffer, usize size) = 0;

        virtual void   Flush() = 0;
        virtual void   Close() = 0;
    };
}