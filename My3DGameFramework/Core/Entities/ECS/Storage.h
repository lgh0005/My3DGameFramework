#pragma once
#include "Entities/ECS/Chunk.h"
#include "Entities/ECS/IStorage.h"

namespace MGF3D
{
	template<typename T>
	class Storage : public IStorage
	{
	public:
		template<typename... Args> T* Emplace(Args&&... args);

	public:
		virtual void Update() override;
		virtual void FixedUpdate() override;
		virtual void LateUpdate() override;

		virtual void Clear() override;
		virtual void Remove(usize index);
		virtual void Defragment();

	private:
		SVector<UniquePtr<Chunk<T>>> m_chunks;
		Ptr<Chunk<T>> m_currentChunk{ nullptr };
		SVector<Ptr<T>> m_denseArray;
	};
}

#include "Entities/ECS/Storage.inl"