#pragma once

CLASS_PTR(Uniformbuffer)
class Uniformbuffer
{
public:
	static UniformbufferUPtr Create(uint32 size, uint32 bindingPoint);
	~Uniformbuffer();

	uint32 Get() const { return m_buffer; }
	void Bind() const;

	// 데이터 전체 혹은 일부를 업데이트하는 핵심 함수
	void SetData(const void* data, uint32 size, uint32 offset = 0);

private:
	Uniformbuffer() = default;
	void Init(uint32 size, uint32 bindingPoint);

	uint32 m_buffer			{ 0 };
	uint32 m_bindingPoint	{ 0 };
	uint32 m_size			{ 0 }; 
};