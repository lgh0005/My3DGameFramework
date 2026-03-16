#pragma once

namespace MGF3D
{
	// INFO : 우리 엔진의 LinearMemoryPool의 로직과 비슷함
	class JoltMemoryPool final : public JPH::TempAllocator
	{
		MGF_DISABLE_COPY(JoltMemoryPool)

	public:
		JoltMemoryPool(usize inCapacity);
		virtual ~JoltMemoryPool() override;

	public:
		virtual void* Allocate(JPH::uint inSize) override;
		virtual void  Free(void* inAddress, JPH::uint inSize) override;

	public:
		void  Reset();
		bool  IsEmpty() const { return m_offset == 0; }
		usize GetUsedMemory() const { return m_offset; }

	private:
		void* m_baseAddress; // 엔진 매니저에게 받아온 거대 버퍼의 시작 주소
		usize m_capacity;    // 버퍼의 총 용량
		usize m_offset;      // 현재까지 사용된 크기 (Stack Top)
	};
}