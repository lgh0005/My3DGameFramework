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
        virtual void Bind() const override;
        void BindBase(uint32 slot) const;
        virtual void Unbind() const override;

    private:
        GLUniformBuffer();
        bool Init
        (
            usize byteSize, 
            VRAMAllocation::PoolType poolType
        );
    };
}