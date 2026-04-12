#include "RuntimePch.h"
#include "Scene.h"
#include "Managers/TypeManager.h"

// #include "Entities/GameObject.h"
//#include "Registries/ComponentRegistry.h"
//#include "Registries/GameObjectRegistry.h"

namespace MGF3D
{
	Scene::Scene() = default;
	Scene::~Scene() = default;

	/*================================//
	//    MGFInputDevice Custom Type  //
	//================================*/
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

	//void Scene::AddGameObject(GameObjectUPtr gameObject)
	//{

	//}

	//void Scene::Destroy(GameObject* obj)
	//{

	//}

	/*======================================//
	//   default scene life-cycle methods   //
	//======================================*/
	void Scene::FixedUpdate()
	{

	}

	void Scene::Update()
	{

	}

	void Scene::LateUpdate()
	{

	}
}