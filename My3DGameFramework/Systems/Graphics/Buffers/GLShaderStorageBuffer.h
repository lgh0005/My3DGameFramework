#pragma once
#include "Buffers/GLBufferHandle.h"

namespace MGF3D
{
    MGF_CLASS_PTR(GLShaderStorageBuffer)
    class GLShaderStorageBuffer : public GLBufferHandle
    {
    public:
        virtual ~GLShaderStorageBuffer() override;
        static GLShaderStorageBufferUPtr Create
        (
            usize byteSize,
            VRAMAllocation::PoolType poolType = VRAMAllocation::PoolType::Dynamic
        );

    public:
        void Bind(uint32 ssboSlot) const;
        void Unbind(uint32 ssboSlot) const;

    private:
        GLShaderStorageBuffer();
        bool Init(usize byteSize, VRAMAllocation::PoolType poolType);
    };
}