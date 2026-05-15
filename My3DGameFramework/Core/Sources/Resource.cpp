#include "CorePch.h"
#include "Resource.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Resource::Resource() = default;
	Resource::~Resource() = default;

	/*==============================//
	//       Resource Type          //
	//==============================*/
	int16 Resource::s_typeIndex = -1;
	const MGFType* Resource::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool Resource::TryBeginSync()
	{
		EResourceState expected = m_state.load(std::memory_order_acquire);
		while (expected != EResourceState::Syncing &&
			expected != EResourceState::Ready &&
			expected != EResourceState::Failed)
		{
			if (m_state.compare_exchange_weak(
				expected, EResourceState::Syncing,
				std::memory_order_acq_rel, std::memory_order_acquire))
			{
				return true;
			}
		}
		return false;
	}
}