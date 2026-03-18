#pragma once
#include "Buffers/GLBufferHandle.h"

namespace MGF3D
{
    MGF_CLASS_PTR(GLShaderStorageBuffer)
    class GLShaderStorageBuffer : public GLBufferHandle
    {
    public:
        virtual ~GLShaderStorageBuffer() override;
        static GLShaderStorageBufferUPtr Create(usize byteSize, uint32 flags);

    public:
        virtual void Bind() const override;
        void BindBase(uint32 slot) const;
        virtual void Unbind() const override;

    private:
        GLShaderStorageBuffer();
        bool Init(usize byteSize, uint32 flags);
    };
}