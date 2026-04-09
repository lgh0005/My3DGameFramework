#include "EnginePch.h"
#include "AnimBuffer.h"

AnimBuffer::AnimBuffer() = default;
AnimBuffer::~AnimBuffer()
{
	if (m_id != 0) glDeleteBuffers(1, &m_id);
}

AnimBufferUPtr AnimBuffer::Create(const void* data, uint32 byteSize)
{
	auto buffer = AnimBufferUPtr(new AnimBuffer());
	if (!buffer->Init(data, byteSize)) return nullptr;
	return std::move(buffer);
}

bool AnimBuffer::Init(const void* data, uint32 byteSize)
{
	if (byteSize == 0) return false;
	m_byteSize = byteSize;

	// 1. 버퍼 객체 생성 (DSA)
	glCreateBuffers(1, &m_id);

	// 2. 메모리 할당 및 데이터 업로드 (Immutable Storage)
	glNamedBufferStorage(m_id, m_byteSize, data, 0);

	return true;
}

void AnimBuffer::Bind(uint32 slot) const
{
	// GL_SHADER_STORAGE_BUFFER (SSBO) 타겟으로 바인딩
	// 셰이더 코드 예: layout(std430, binding = 10) buffer AnimAtlas { ... };
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_id);
}

void AnimBuffer::Unbind(uint32 slot) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
}