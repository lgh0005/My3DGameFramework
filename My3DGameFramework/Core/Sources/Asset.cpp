#include "CorePch.h"
#include "Asset.h"
#include "Resource.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Asset::Asset(const String& path) : m_path(path) { }
	Asset::~Asset() = default;

	/*==============================//
	//       Asset Type             //
	//==============================*/
	int16 Asset::s_typeIndex = -1;
	const MGFType* Asset::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Asset");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void Asset::AddResource(const ResourcePtr& resource)
	{
		m_resources.push_back(resource);
	}

	EAssetState Asset::GetState() const
	{
		EAssetState state = m_state.load(std::memory_order_acquire);

		// 에셋 자체 상태가 이미 Failed라면 즉시 반환
		if (m_state == EAssetState::Failed) return EAssetState::Failed;

		// 만약 CPU 로딩이 끝난 상태라면, 내부 리소스들의 상태를 합산해서 판단
		if (m_state == EAssetState::Loaded || state == EAssetState::Syncing)
		{
			bool isAllReady = true;
			for (const auto& res : m_resources)
			{
				auto resState = res->GetState(); 
				if (resState == EResourceState::Failed) return EAssetState::Failed;
				if (resState != EResourceState::Ready) isAllReady = false;
			}

			// 모든 리소스가 Ready라면 에셋도 Ready인 셈
			if (isAllReady) return EAssetState::Ready;
		}

		return m_state;
	}
}