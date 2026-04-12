#include "GamePch.h"
#include "TestScene.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	TestScene::TestScene() = default;
	TestScene::~TestScene() = default;

	/*================================//
	//    MGFInputDevice Custom Type  //
	//================================*/
	int16 TestScene::s_typeIndex = -1;
	const MGFType* TestScene::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Scene");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool TestScene::OnLoadSceneResources()
	{
		return true;
	}

	bool TestScene::OnPlaceActors()
	{
		return true;
	}
}