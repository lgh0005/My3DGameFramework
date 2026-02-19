#pragma once
#include "Graphics/Buffers/Buffer.h"

CLASS_PTR(VertexBuffer)
class VertexBuffer : public Buffer
{
public:
	virtual ~VertexBuffer();
	static VertexBufferUPtr Create
	(
		const void* data,
		usize stride,
		usize count,
		uint32 usage = GL_STATIC_DRAW
	);

private:
	VertexBuffer();

};