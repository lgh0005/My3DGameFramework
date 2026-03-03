#include "EnginePch.h"
#include "VertexBuffer.h"

namespace MGF3D
{
	VertexBuffer:VertexBuffer() = default;
	VertexBuffer::~VertexBuffer() = default;

	VertexBufferUPtr VertexBuffer::Create
	(
		const void* data,
		usize stride,
		usize count,
		uint32 usage
	)
	{
		auto buffer = VertexBufferUPtr(new VertexBuffer());
		if (!buffer->Init(GL_ARRAY_BUFFER, usage, data, stride, count)) return nullptr;
		return std::move(buffer);
	}
}