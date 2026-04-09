#pragma once

CLASS_PTR(AnimBuffer)
class AnimBuffer
{
public:
	~AnimBuffer();
	static AnimBufferUPtr Create(const void* data, uint32 byteSize);

	void Bind(uint32 slot) const;
	void Unbind(uint32 slot) const;

	uint32 GetID() const { return m_id; }
	uint32 GetByteSize() const { return m_byteSize; }

private:
	AnimBuffer();
	bool Init(const void* data, uint32 byteSize);

	uint32 m_id{ 0 };
	uint32 m_byteSize{ 0 };
};