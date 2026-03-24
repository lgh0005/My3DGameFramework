#pragma once
#include "Stream/Stream.h"

namespace MGF3D
{
    MGF_CLASS_PTR(MemoryStreamBuffer)

    MGF_CLASS_PTR(MemoryStream)
	class MemoryStream : public Stream 
	{
	public:
        MemoryStream(MemoryStreamBufferPtr buffer, usize dataSize = 0);
		virtual ~MemoryStream() override;

	public:
        virtual bool   CanRead() const override { return true; }
        virtual bool   CanWrite() const override { return true; }
        virtual bool   CanSeek() const override { return true; }

        virtual uint64 GetLength() const override;
        virtual uint64 GetPosition() const override;
        virtual void   SetPosition(uint64 pos) override;

        virtual usize  Read(void* buffer, usize size) override;
        virtual usize  Write(const void* buffer, usize size) override;

        virtual void   Flush() override { }
        virtual void   Close() override;

    public:
        const byte* GetData() const;

    private:
        MemoryStreamBufferPtr  m_buffer   { nullptr };
        uint64                 m_position { 0 };
        uint64                 m_length{ 0 };
    };
}