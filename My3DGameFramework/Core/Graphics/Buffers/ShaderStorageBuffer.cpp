#include "EnginePch.h"
#include "Graphics/Buffers/ShaderStorageBuffer.h"

DECLARE_DEFAULTS_IMPL(ShaderStorageBuffer)

ShaderStorageBufferUPtr ShaderStorageBuffer::Create(usize byteSize, uint32 usage)
{
	auto buffer = ShaderStorageBufferUPtr(new ShaderStorageBuffer());
	if (!buffer->Init(GL_SHADER_STORAGE_BUFFER, usage, nullptr, 1, byteSize))
		return nullptr;

	return std::move(buffer);
}

ShaderStorageBufferUPtr ShaderStorageBuffer::CreateWithData(const void* data, usize byteSize, uint32 usage)
{
	auto buffer = ShaderStorageBufferUPtr(new ShaderStorageBuffer());
	if (!buffer->Init(GL_SHADER_STORAGE_BUFFER, usage, data, 1, byteSize))
		return nullptr;

	return std::move(buffer);
}