#include "CorePch.h"
#include "IStorage.h"

namespace MGF3D
{
	void IStorage::MarkStorageDirty()
	{
		m_isStorageDirty = true;
	}

	bool IStorage::IsDirty() const
	{
		return m_isStorageDirty;
	}

	usize IStorage::GetActiveCount() const
	{
		return m_activeCount;
	}
}