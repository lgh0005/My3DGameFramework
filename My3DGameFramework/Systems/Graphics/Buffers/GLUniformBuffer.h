#pragma once
#include "Graphics/Buffers/GLBufferHandle.h"

namespace MGF3D
{
    MGF_CLASS_PTR(GLUniformBuffer)
    class GLUniformBuffer : public GLBufferHandle
    {
    public:
        virtual ~GLUniformBuffer() override;
        static GLUniformBufferUPtr Create(usize byteSize, uint32 flags);

    public:
        virtual void Bind() const override;
        void BindBase(uint32 slot) const;
        virtual void Unbind() const override;

    private:
        GLUniformBuffer();
        bool Init(usize byteSize, uint32 flags);
    }
}