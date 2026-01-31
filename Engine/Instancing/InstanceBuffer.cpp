#include "EnginePch.h"
#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer() = default;
InstanceBuffer::~InstanceBuffer()
{
	if (m_id != 0) glDeleteBuffers(1, &m_id);
}

InstanceBufferUPtr InstanceBuffer::Create(uint32 maxCount, uint32 structSize)
{
	auto buffer = InstanceBufferUPtr(new InstanceBuffer());
	if (!buffer->Init(maxCount, structSize)) return nullptr;
	return std::move(buffer);
}

bool InstanceBuffer::Init(uint32 maxCount, uint32 structSize)
{
	m_maxCount = maxCount;
	m_structSize = structSize;

	// 1. 버퍼 생성 (DSA 방식)
	glCreateBuffers(1, &m_id);

	// 2. 메모리 할당 (수정이 잦으므로 GL_DYNAMIC_STORAGE_BIT 사용)
	uint32 totalSize = m_maxCount * m_structSize;
	glNamedBufferStorage(m_id, totalSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

	return true;
}

void InstanceBuffer::Update(const void* data, uint32 size)
{
	if (size == 0) return;
	glNamedBufferSubData(m_id, 0, size, data);
}

void InstanceBuffer::Bind(uint32 slot) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_id);
}

void InstanceBuffer::Unbind(uint32 slot) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
}