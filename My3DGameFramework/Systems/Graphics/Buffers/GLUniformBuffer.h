#pragma once
#include "Buffers/GLBufferHandle.h"

namespace MGF3D
{
    MGF_CLASS_PTR(GLUniformBuffer)
        class GLUniformBuffer : public GLBufferHandle
    {
    public:
        virtual ~GLUniformBuffer() override;
        static GLUniformBufferUPtr Create
        (
            usize byteSize, 
            VRAMAllocation::PoolType poolType = VRAMAllocation::PoolType::Dynamic
        );

    public:
        void Bind(uint32 uboSlot) const;
        void Unbind(uint32 uboSlot) const;

    private:
        GLUniformBuffer();
        bool Init
        (
            usize byteSize, 
            VRAMAllocation::PoolType poolType
        );
    };
}