#include "EnginePch.h"
#include "UniformBuffer.h"

DECLARE_DEFAULTS_IMPL(UniformBuffer)

UniformBufferUPtr UniformBuffer::Create(usize byteSize, uint32 usage)
{
	auto buffer = UniformBufferUPtr(new UniformBuffer());
	if (!buffer->Init(GL_UNIFORM_BUFFER, usage, nullptr, 1, byteSize))
		return nullptr;
	return std::move(buffer);
}

UniformBufferUPtr UniformBuffer::CreateWithData(const void* data, usize byteSize, uint32 usage)
{
	auto buffer = UniformBufferUPtr(new UniformBuffer());
	if (!buffer->Init(GL_UNIFORM_BUFFER, usage, data, 1, byteSize))
		return nullptr;
	return std::move(buffer);
}