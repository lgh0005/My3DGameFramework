#pragma once

namespace MGF3D
{
	template<typename T>
	class MGFPackedArray
	{
	private:
		Vector<T> m_data;
		HashMap<int32, int32> m_entityToIndex;
		HashMap<int32, int32> m_indexToEntity;

	public:
		MGFPackedArray();
		~MGFPackedArray();

	public:
		template <typename... Args> T& Emplace(int32 id, Args&&... args);
		void RemoveOnEntityDestroyed(int32 id) override;
		void Clear();

	public:
		T* Get(int32 id);
		bool Has(int32 id) const;

	public:
		auto begin() { return m_data.begin(); }
		auto end() { return m_data.end(); }
		auto begin() const { return m_data.begin(); }
		auto end() const { return m_data.end(); }

		T* GetData() { return m_data.data(); }
		usize GetSize() const { return m_data.size(); }
	};
}

#include "Types/MGFPackedArray.inl"