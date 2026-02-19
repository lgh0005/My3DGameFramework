#pragma once
#include "Graphics/Buffers/Buffer.h"

CLASS_PTR(IndexBuffer)
class IndexBuffer : public Buffer
{
public:
	virtual ~IndexBuffer() override;

	// 1. 편의성 오버로딩: 벡터를 바로 받음
	static IndexBufferUPtr Create
	(
		const std::vector<uint32>& indices,
		uint32 usage = GL_STATIC_DRAW
	);

	// 2. 기본 생성: 포인터와 개수 받음
	static IndexBufferUPtr Create
	(
		const void* data,
		usize count,
		uint32 usage = GL_STATIC_DRAW
	);

private:
	IndexBuffer();
};