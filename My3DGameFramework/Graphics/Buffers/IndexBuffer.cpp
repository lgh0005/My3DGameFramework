#include "EnginePch.h"
#include "IndexBuffer.h"

DECLARE_DEFAULTS_IMPL(IndexBuffer)

IndexBufferUPtr IndexBuffer::Create(const std::vector<uint32>& indices, uint32 usage)
{
	return Create(indices.data(), indices.size(), usage);
}

IndexBufferUPtr IndexBuffer::Create(const void* data, usize count, uint32 usage)
{
	auto buffer = IndexBufferUPtr(new IndexBuffer());
	if (!buffer->Init(GL_ELEMENT_ARRAY_BUFFER, usage, data, sizeof(uint32), count))
		return nullptr;
	return std::move(buffer);
}