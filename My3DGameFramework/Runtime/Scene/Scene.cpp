#include "RuntimePch.h"
#include "Scene.h"
#include "Managers/TypeManager.h"
#include "Managers/EntityManager.h"

namespace MGF3D
{
	Scene::Scene() = default;
	Scene::~Scene()
	{
		Clear();
	}

	/*=========================//
	//    Scene Custom Type    //
	//=========================*/
	int16 Scene::s_typeIndex = -1;
	const MGFType* Scene::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Scene");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	/*======================================//
	//   default scene GameObject methods   //
	//======================================*/
	bool Scene::Init()
	{
		// 1. 엔진 레벨 리소스 로드
		if (!OnLoadSceneResources()) return false;

		// 2. Actor들을 배치
		if (!OnPlaceActors()) return false;

		return true;
	}

	void Scene::Update()
	{
		MGF_ENTITY.Update();
	}

	void Scene::Clear()
	{
		MGF_ENTITY.Clear();
	}
}