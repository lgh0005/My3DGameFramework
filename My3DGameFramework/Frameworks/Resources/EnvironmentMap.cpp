#include "FrameworkPch.h"
#include "EnvironmentMap.h"
#include "Managers/TypeManager.h"
#include "Assets/Image.h"

namespace MGF3D
{
	EnvironmentMap::EnvironmentMap(StringView name) : Super(name) { }
	EnvironmentMap::~EnvironmentMap() = default;

	/*========================//
	//     Material Type      //
	//========================*/
	int16 EnvironmentMap::s_typeIndex = -1;
	const MGFType* EnvironmentMap::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	EnvironmentMapPtr EnvironmentMap::Create(StringView mapName)
	{
		auto envMap = EnvironmentMapPtr(new EnvironmentMap(mapName));
		envMap->SetState(EResourceState::Loaded);
		return envMap;
	}

	bool EnvironmentMap::OnSyncCreate()
	{
		// TODO : 여기에서 비동기가 일어남.
		// 다만, vao와 fbo는 공유가 안된다고 하니 그 점 유의할 것.
		return true;
	}

	void EnvironmentMap::SetCubeTexture(const ImagePtr& image)
	{
		m_environmentCubeImage = image;
	}
}