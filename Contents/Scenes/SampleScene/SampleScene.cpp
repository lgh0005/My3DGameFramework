#include "GamePch.h"
#include "SampleScene.h"
#include "Assets/Scripts/CameraController.h"

namespace MGF3D
{
	SampleScene::SampleScene() = default;
	SampleScene::~SampleScene() = default;

	/*================================//
	//    MGFInputDevice Custom Type  //
	//================================*/
	int16 SampleScene::s_typeIndex = -1;
	const MGFType* SampleScene::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Scene");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool SampleScene::OnLoadSceneSources()
	{
		auto sharedMaterial = MGF_RESOURCE.GetOrCreate<Material>("SharedCubeMaterial");
		sharedMaterial->SetTexture(ETextureSlot::Albedo, MGF_ASSET.LoadAssetAsync<Image>("@GameAsset/Images/brickwall.ktx"));
		sharedMaterial->SetTexture(ETextureSlot::Normal, MGF_ASSET.LoadAssetAsync<Image>("@GameAsset/Images/brickwall_normal.ktx"));

		MGF_ASSET.LoadAssetAsync<Model>("@GameAsset/Models/backpack/backpack.mymodel");
		
		MGF_ASSET.LoadAssetAsync<Model>("@GameAsset/Models/test/aliensoldier.mymodel");
		MGF_ASSET.LoadAssetAsync<Animation>("@GameAsset/Models/test/Hip Hop Dancing.myanim");

		return true;
	}

	bool SampleScene::OnPlaceActors()
	{
		// 1. 객체 생성 [카메라]
		ObjectIDHash playerID = Entities::Create("MainPlayer");
		auto* transform = Entities::AddComponent<Transform>(playerID);
		auto* camera = Entities::AddComponent<Camera>(playerID);
		auto* controller = Scripts::AddScript<CameraController>(playerID);
		transform->SetLocalPosition(vec3(0.0f, 0.0f, 5.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		// [핵심] 자원은 메모리에 딱 한 번만 생성하여 공유합니다.
		StaticMeshPtr sharedMesh = GeometryUtils::CreateSphere();
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

		// 4. 객체 생성 [포인트 라이트]
		ObjectIDHash lightID = Entities::Create("PointLight1");
		auto* lightTransform = Entities::AddComponent<Transform>(lightID);
		auto* pointLight = Entities::AddComponent<PointLight>(lightID);
		lightTransform->SetLocalPosition(vec3(0.0f, 1.0f, 2.0f));
		pointLight->SetColor(vec3(0.8f, 0.8f, 1.0f));
		pointLight->SetIntensity(2.0f);
		pointLight->SetRange(30.0f);

		// 5. 가방
		auto bag = MGF_ASSET.GetAsset<Model>("@GameAsset/Models/backpack/backpack.mymodel");
		ObjectIDHash bagID = bag->Instantiate("backpack");
		auto* bagTransform = Entities::GetComponent<Transform>(bagID);
		bagTransform->SetLocalPosition(vec3(0.0f, 1.0f, 0.0f));
		bagTransform->SetLocalScale(vec3(1.0f));

		// 6. AilenSoldier
		auto soldier = MGF_ASSET.GetAsset<Model>("@GameAsset/Models/test/aliensoldier.mymodel");
		ObjectIDHash soldierID = soldier->Instantiate("Soldier");
		auto* soldierTransform = Entities::GetComponent<Transform>(soldierID);
		soldierTransform->SetLocalPosition(vec3(1.5f, 0.0f, 0.0f));
		soldierTransform->SetLocalRotation(vec3(0.0f, 90.0f, 0.0f));
		soldierTransform->SetLocalScale(vec3(0.01f));

		auto danceAsset = MGF_ASSET.GetAsset<Animation>("@GameAsset/Models/test/Hip Hop Dancing.myanim");
		auto animController = MakeUnique<AnimController>(nullptr);
		animController->AddState("Dance", danceAsset, true, 1.0f);
		animController->SetStartState(StringHash("Dance"));
		animController->Play(StringHash("Dance"));

		auto* soldierAnimator = Entities::AddComponent<Animator>(soldierID, soldier, std::move(animController));

		return true;
	}
}