#pragma once

namespace MGF3D
{
	class IStorage
	{
	public:
		virtual ~IStorage() = default;

		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;
		virtual void LateUpdate() = 0;
		virtual void Clear() = 0;
		virtual void Remove(usize index) = 0;

		virtual void Defragment() = 0;

		void MarkDirty() { m_isDirty = true; }
		bool IsDirty() const { return m_isDirty; }
		usize GetActiveCount() const { return m_activeCount; }

	protected:
		usize m_activeCount = 0;
		bool  m_isDirty = false;
	};
}