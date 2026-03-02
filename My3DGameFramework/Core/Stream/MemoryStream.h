#pragma once
#include "Stream/Stream.h"

namespace MGF3D
{
	class MemoryStream : public Stream 
	{
	public:
		virtual ~MemoryStream();

	public:
        virtual bool   CanRead() const override;
        virtual bool   CanWrite() const override;
        virtual bool   CanSeek() const override;

        virtual uint64 GetLength() const override;
        virtual uint64 GetPosition() const override;
        virtual void   SetPosition(uint64 pos) override;

        virtual usize  Read(void* buffer, usize size) override;
        virtual usize  Write(const void* buffer, usize size) override;

        virtual void   Flush() override;
        virtual void   Close() override;
	};
}