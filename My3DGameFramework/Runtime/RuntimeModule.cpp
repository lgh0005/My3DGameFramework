#include "RuntimePch.h"
#include "RuntimeModule.h"
#include "Managers/TypeManager.h"
#include "Scene/Scene.h"

namespace MGF3D
{
	void RuntimeModule::OnRegisterTypes()
	{
		MGFTypeTree& sceneTree = MGF_TYPE.CreateTree("Scene");
		Scene::s_typeIndex = sceneTree.Register("Scene", "");
	}

	bool RuntimeModule::OnInit()
	{
		return true;
	}

	bool RuntimeModule::OnShutdown()
	{
		return true;
	}
}