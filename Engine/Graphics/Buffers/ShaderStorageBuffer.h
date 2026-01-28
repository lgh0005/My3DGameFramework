#pragma once
#include "Graphics/Buffers/Buffer.h"

CLASS_PTR(ShaderStorageBuffer)
class ShaderStorageBuffer : public Buffer
{
public:
	virtual ~ShaderStorageBuffer() override;

	// 1. 빈 버퍼 생성 (데이터 수집용 / 셰이더에서 쓰기용)
	// 예: OIT(Order Independent Transparency), GPU Particle, Compute Shader 결과 저장
	static ShaderStorageBufferUPtr Create
	(
		usize byteSize, 
		uint32 usage = GL_DYNAMIC_DRAW
	);

	// 2. 초기 데이터와 함께 생성 (구조체 배열 등)
	// 예: Light 데이터 배열(개수 제한 없는), 인스턴싱 데이터
	static ShaderStorageBufferUPtr CreateWithData
	(
		const void* data, 
		usize byteSize, 
		uint32 usage = GL_STATIC_DRAW
	);

private:
	ShaderStorageBuffer();
};