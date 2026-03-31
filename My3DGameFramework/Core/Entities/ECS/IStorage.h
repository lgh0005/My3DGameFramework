#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(IStorage)
	class IStorage
	{
	public:
		virtual ~IStorage() = default;

		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;
		virtual void LateUpdate() = 0;
		virtual void Clear() = 0;

	public:
		// 업데이트 간에 업데이트 할 것과 그렇지 않은 것 구분하는 메서드
		virtual void Defragment() = 0;
		virtual void Remove(usize index) = 0;

	public:
		void MarkStorageDirty();
		bool IsDirty()		     const;
		usize GetActiveCount()   const;

	protected:
		usize m_activeCount = 0;
		bool  m_isStorageDirty = false;
	};
}