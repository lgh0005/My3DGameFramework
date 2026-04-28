#pragma once
#include "ECS/MGFChunk.h"
#include "Constants/ConstantChunk.h"

namespace MGF3D
{
	template<typename T>
	class MGFPackedArray
	{
	private:
		// 실제 데이터가 담긴 고정된 메모리 블록들
		using ChunkType = MGFChunk<T, CHUNK_SIZE>;
		Vector<UniquePtr<ChunkType>> m_storage;

		// 2. 순회 및 그룹화를 위한 포인터 배열(Packed Array)
		Vector<T*> m_data;

		// 3. 매핑 정보
		HashMap<int32, int32> m_entityToIndex;
		HashMap<int32, int32> m_indexToEntity;
		HashMap<int32, usize> m_entityToStorageIndex;

		// 4. 삭제된 칸 재사용을 위한 Free List
		Vector<usize> m_freeSlots;
		int32 m_boundaryIndex{ 0 };
		usize m_totalStorageCapacity{ 0 };

	public:
		MGFPackedArray();
		~MGFPackedArray();

	public:
		template <typename... Args> T& Emplace(int32 id, Args&&... args);
		void Clear();

	public:
		void MoveToFrontGroup(int32 id);
		void MoveToBackGroup(int32 id);
		int32 GetFrontGroupSize() const { return m_boundaryIndex; }
		void Remove(int32 id);

	public:
		T* Get(int32 id);
		bool Has(int32 id) const;

	public:
		auto begin() { return m_data.begin(); }
		auto end() { return m_data.end(); }
		auto begin() const { return m_data.begin(); }
		auto end() const { return m_data.end(); }

		T** GetData() { return m_data.data(); }
		usize GetSize() const { return m_data.size(); }
	};
}

#include "ECS/MGFPackedArray.inl"