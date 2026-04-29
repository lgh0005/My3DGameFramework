#include "GamePch.h"
#include "TestScene.h"
#include "Managers/TypeManager.h"
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/MeshRenderers/MeshRenderer.h"

#include "Meshes/StaticMesh.h"
#include "Resources/Material.h"
#include "GraphicsUtils/GeometryUtils.h"

#include "Scripts/CameraController.h"

#include "Assets/Image.h"

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

	bool TestScene::OnLoadSceneSources()
	{
		auto sharedMaterial = MGF_RESOURCE.GetOrCreate<Material>("SharedCubeMaterial");
		sharedMaterial->SetTexture
		(
			ETextureSlot::Albedo,
			MGF_ASSET.LoadAssetAsync<Image>("@GameAsset/Images/baked/brickwall.ktx")
		);

		return true;
	}

	bool TestScene::OnPlaceActors()
	{
		// 1. 객체 생성 [카메라]
		ObjectIDHash playerID = Entities::Create("MainPlayer");
		auto* transform = Entities::AddComponent<Transform>(playerID);
		auto* camera = Entities::AddComponent<Camera>(playerID);
		auto* controller = Scripts::AddScript<CameraController>(playerID);
		transform->SetLocalPosition(vec3(0.0f, 0.0f, 5.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		// [핵심] 자원은 메모리에 딱 한 번만 생성하여 공유합니다.
		StaticMeshPtr sharedMesh = GeometryUtils::CreateCone();
		MaterialPtr sharedMaterial = MGF_RESOURCE.Get<Material>("SharedCubeMaterial");

		// 2. 객체 생성 [첫 번째 큐브]
		ObjectIDHash cubeID1 = Entities::Create("Cube1");
		auto* cubeTransform1 = Entities::AddComponent<Transform>(cubeID1);
		Entities::AddComponent<MeshRenderer>(cubeID1, sharedMesh, sharedMaterial);
		cubeTransform1->SetLocalPosition(vec3(-1.5f, 0.0f, 0.0f));

		// 3. 객체 생성 [두 번째 큐브]
		ObjectIDHash cubeID2 = Entities::Create("Cube2");
		auto* cubeTransform2 = Entities::AddComponent<Transform>(cubeID2);
		Entities::AddComponent<MeshRenderer>(cubeID2, sharedMesh, sharedMaterial);
		cubeTransform2->SetLocalPosition(vec3(1.5f, 0.0f, 0.0f));

		return true;
	}
}