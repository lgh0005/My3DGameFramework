#pragma once
#include "Graphics/Buffers/Buffer.h"

CLASS_PTR(UniformBuffer)
class UniformBuffer : public Buffer
{
public:
	virtual ~UniformBuffer() override;

	// 1. 데이터 없이 공간만 할당 (매 프레임 갱신용 - 예: Camera, Light)
	static UniformBufferUPtr Create
	(
		usize byteSize, 
		uint32 usage = GL_DYNAMIC_DRAW
	);

	// 2. 초기 데이터와 함께 생성 (거의 안 변하는 데이터용 - 예: Material Constants)
	static UniformBufferUPtr CreateWithData
	(
		const void* data, 
		usize byteSize, 
		uint32 usage = GL_STATIC_DRAW
	);

private:
	UniformBuffer();
};