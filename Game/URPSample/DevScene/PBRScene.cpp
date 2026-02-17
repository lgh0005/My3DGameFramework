#include "EnginePch.h"
#include "PBRScene.h"

#include "Object/GameObject.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Resources/Animations/Animation.h"
#include "Resources/Model.h"
#include "Resources/Meshes/InstancedMesh.h"
#include "Components/SkyLight.h"
#include "Resources/AudioClip.h"
#include "Resources/Animations/AnimController.h"
#include "Resources/EnvironmentMap.h"

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/SpotLight.h"
#include "Components/DirectionalLight.h"
#include "Components/PointLight.h"
#include "Components/Animator.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"
#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/CapsuleCollider.h"

#include "URPSample/RenderPasses/HDRRenderPass.h"
//#include "SRPSample/Scripts/CameraController.h"
//#include "SRPSample/Scripts/PlayerController.h"

DECLARE_DEFAULTS_IMPL(PBRScene)

PBRSceneUPtr PBRScene::Create()
{
	return PBRSceneUPtr(new PBRScene());
}

bool PBRScene::LoadSceneResources()
{
	// [Texture] Rusted Iron (KTX)
	/*RESOURCE.Add<Texture>("rusted_base", "@GameAsset/Images/rustediron/rustediron2_basecolor.ktx");
	RESOURCE.Add<Texture>("rusted_normal", "@GameAsset/Images/rustediron/rustediron2_normal.ktx");
	RESOURCE.Add<Texture>("rusted_orm", "@GameAsset/Images/rustediron/rustediron2_ORM.ktx");*/

	// [Texture] ToyBox (KTX)
	RESOURCE.Add<Texture>("toybox_diffuse", "@GameAsset/Images/baked/toy_box_diffuse.ktx");
	RESOURCE.Add<Texture>("toybox_normal", "@GameAsset/Images/baked/toy_box_normal.ktx");
	RESOURCE.Add<Texture>("toybox_disp", "@GameAsset/Images/baked/toy_box_disp.ktx");

	// [Texture] HDR Source (IBL용)
	RESOURCE.Add<Texture>("mirrored_hall", "@GameAsset/Images/baked/mirrored_hall_4k.ktx");

	// [Model & Animation]
	RESOURCE.Add<Model>("aliensoldier", "@GameAsset/Models/spacesoldier/aliensoldier.mymodel");
	RESOURCE.Add<Model>("backpack", "@GameAsset/Models/backpack/backpack.mymodel");
	RESOURCE.Add<Animation>("Idle", "@GameAsset/Models/spacesoldier/Idle_shorten.myanim");
	RESOURCE.Add<Animation>("Walk", "@GameAsset/Models/spacesoldier/Walking.myanim");

	// [Material]
	RESOURCE.Add<Material>("rusted_iron");

	// 1. 단색 머티리얼 (Texture::CreateSolid 사용)
	{
		auto mat = Material::Create();
		mat->GetDesc().name = "solidColor";
		mat->GetDesc().path = "@Virtual/Materials/solidColor";
		mat->diffuse = Texture::CreateSolid({ 191, 25, 51, 255 });
		mat->metallic = Texture::CreateSolid({ 217, 217, 217, 255 });
		mat->roughness = Texture::CreateSolid({ 64, 89, 64, 255 });
		RESOURCE.Register<Material>(std::move(mat));
	}

	// 3. 쇠공 머티리얼 #2 (ORM Texture)
	{
		auto mat = Material::Create();
		mat->GetDesc().name = "Rusted_Iron_orm";
		mat->GetDesc().path = "@Virtual/Materials/Rusted_Iron_orm";
		mat->diffuse = RESOURCE.Get<Texture>("rusted_base");
		mat->normal = RESOURCE.Get<Texture>("rusted_normal");
		mat->orm = RESOURCE.Get<Texture>("rusted_orm");
		RESOURCE.Register<Material>(std::move(mat));
	}

	// 4. HDR 큐브맵 확인용 머티리얼
	{
		auto mat = Material::Create();
		mat->GetDesc().name = "hdrCubeMat";
		mat->GetDesc().path = "@Virtual/Materials/hdrCubeMat";
		mat->diffuse = RESOURCE.Get<Texture>("mirrored_hall");
		RESOURCE.Register<Material>(std::move(mat));
	}

	// 5. 패럴랙스 매핑용 머티리얼
	{
		auto mat = Material::Create();
		mat->GetDesc().name = "boxMat5";
		mat->GetDesc().path = "@Virtual/Materials/boxMat5";
		mat->diffuse = RESOURCE.Get<Texture>("toybox_diffuse");
		mat->normal = RESOURCE.Get<Texture>("toybox_normal");
		mat->height = RESOURCE.Get<Texture>("toybox_disp");
		mat->shininess = 14.0f;
		mat->emissionStrength = 0.0f;
		mat->heightScale = 0.065f;
		RESOURCE.Register<Material>(std::move(mat));
	}

	// 6. 빈 머티리얼 (Ground)
	{
		auto mat = Material::Create();
		mat->GetDesc().name = "GroundMat";
		mat->GetDesc().path = "@Virtual/Materials/GroundMat";
		RESOURCE.Register<Material>(std::move(mat));
	}

	// 7. Environment Map 생성 (UniversalSky)
	{
		EnvironmentMapDesc envDesc;
		envDesc.name = "mirrored_hall";
		auto envMap = EnvironmentMap::Load(envDesc);
		if (envMap)
		{
			envMap->GetDesc().name = "UniversalSky";
			RESOURCE.Register<EnvironmentMap>(std::move(envMap));
		}
	}

	return true;
}

bool PBRScene::CreateCustomRenderPasses()
{
	auto prog = RESOURCE.Add<GraphicsProgram>
	(
		"ForwardPBR",
		"@GameAsset/Shaders/Forward_PBR.vert",
		"@GameAsset/Shaders/Forward_PBR.frag"
	);

	if (prog) AddRenderPass("simpleHDR", HDRRenderPass::Create(prog));

	return true;
}

bool PBRScene::OnPlaceActors()
{
	// 0. 추가한 렌더패스 가져오기
	HDRRenderPass* hdrPass = (HDRRenderPass*)GetRenderPass("simpleHDR");

	// 1. 카메라 오브젝트 추가
	{
		// GameObject 생성 및 씬에 등록
		auto cameraObj = GameObject::Create(); if (!cameraObj) return false;
		auto camera = Camera::Create();		   if (!camera)	   return false;
		auto* cameraPtr = camera.get();

		// 카메라 컴포넌트 생성 및 추가
		cameraObj->GetTransform().SetPosition(glm::vec3(0.0f, 2.5f, 8.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 100.0f);
		cameraObj->AddComponent(std::move(camera));

		// 카메라 컨트롤러 생성 및 추가
		/*auto cameraCtrl = CameraController::Create();
		cameraObj->AddComponent(std::move(cameraCtrl));*/

		AddGameObject(std::move(cameraObj));
	}

	// 2. 하늘 GameObject 생성
	{
		auto skyObj = GameObject::Create();
		auto sky = SkyLight::Create(RESOURCE.Get<EnvironmentMap>("UniversalSky"));
		skyObj->AddComponent(std::move(sky));
		AddGameObject(std::move(skyObj));
	}

	// 3. 조명 추가 (Point Lights)
	{
		// Helper Lambda for creating point lights
		auto AddPointLight = [&](const std::string& name, glm::vec3 pos) 
		{
			auto lightGo = GameObject::Create();
			lightGo->SetName(name);
			auto lightComp = PointLight::Create();
			lightGo->GetTransform().SetPosition(pos);
			lightGo->GetTransform().SetScale(glm::vec3(0.2f));
			lightGo->AddComponent(std::move(lightComp));
			AddGameObject(std::move(lightGo));
		};

		AddPointLight("PointLight1", glm::vec3(5.0f, 5.0f, 4.0f));
		AddPointLight("PointLight2", glm::vec3(-4.0f, 5.0f, 5.0f));
		AddPointLight("PointLight3", glm::vec3(-4.0f, -6.0f, 6.0f));
		AddPointLight("PointLight4", glm::vec3(5.0f, -6.0f, 7.0f));
	}

	// 4. 그림자가 있는 SpotLight + 큐브
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("SpotLight");
		auto lightComp = SpotLight::Create();
		lightComp->SetCastShadow(true);
		lightComp->SetIntensity(10.0f);
		lightGo->GetTransform().SetPosition(glm::vec3(1.0f, 4.0f, 4.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightComp->SetCutoff(glm::vec2(60.0, 5.0f));
		lightComp->SetDistance(128.0f);
		lightGo->AddComponent(std::move(lightComp));
		
		auto renderer = StaticMeshRenderer::Create
		(RESOURCE.Get<StaticMesh>("Cube"), RESOURCE.Get<Material>("solidColor"));
		lightGo->AddComponent(std::move(renderer));

		AddGameObject(std::move(lightGo));
	}

	// 5. 바닥 큐브 (Ground)
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Ground");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
		cubeTransform.SetScale(glm::vec3(40.0f, 1.0f, 40.0f));

		auto collider = BoxCollider::Create(glm::vec3(40.0f, 1.0f, 40.0f));
		cubeObj->AddComponent(std::move(collider));

		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Static);
		rb->SetRestitution(0.5f);
		rb->SetFriction(0.5f);
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.Get<StaticMesh>("Cube"), RESOURCE.Get<Material>("GroundMat"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 6. 패럴랙스 매핑 블록
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("ToyBlock");
		auto& transform = cubeObj->GetTransform();
		transform.SetPosition(glm::vec3(-3.0f, 3.0f, 1.0f));
		transform.SetRotation(glm::vec3(45.0f));
		transform.SetScale(glm::vec3(0.75f));

		auto collider = BoxCollider::Create(glm::vec3(0.75f));
		cubeObj->AddComponent(std::move(collider));

		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Static);
		rb->SetRestitution(0.5f);
		rb->SetFriction(0.5f);
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.Get<StaticMesh>("Cube"), RESOURCE.Get<Material>("boxMat5"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 7. Alien Soldier 모델
	{
		auto model = RESOURCE.Get<Model>("aliensoldier");
		auto anim1 = RESOURCE.Get<Animation>("Idle");
		auto anim2 = RESOURCE.Get<Animation>("Walk");

		auto animCtrl = AnimController::Create();
		animCtrl->AddState("Idle", anim1);
		animCtrl->AddState("Walk", anim2);
		animCtrl->SetTransitionDuration("Idle", "Walk", 0.2f);
		animCtrl->SetTransitionDuration("Walk", "Idle", 0.2f);
		animCtrl->SetStartState("Idle");

		auto animator = Animator::Create(model, std::move(animCtrl));

		auto boxCollider = CapsuleCollider::Create(0.35f, 4.5f);
		boxCollider->SetTrigger(false);
		boxCollider->SetOffset(glm::vec3(0.0f, 2.25f, 0.0f));

		auto rigidBody = Rigidbody::Create();
		rigidBody->SetMotionType(JPH::EMotionType::Dynamic);
		rigidBody->SetUseGravity(true);
		rigidBody->SetMass(76.0f);
		rigidBody->FreezeRotation(true, true, true);
		rigidBody->SetFriction(0.5f);

		/*auto playerctrl = PlayerController::Create();*/

		GameObjectUPtr rootUPtr = model->Instantiate(this, animator.get());
		GameObject* rootGO = rootUPtr.get();
		if (rootGO)
		{
			rootGO->SetName("Soldier");
			rootGO->GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -2.0f));
			rootGO->GetTransform().SetScale(glm::vec3(0.025f));

			rootGO->AddComponent(std::move(animator));
			/*rootGO->AddComponent(std::move(playerctrl));*/
			rootGO->AddComponent(std::move(boxCollider));
			rootGO->AddComponent(std::move(rigidBody));
		}
		AddGameObject(std::move(rootUPtr));
	}

	// 3. 구 49개 (ORM 텍스쳐 테스트)
	TestSpheresForORMTexture(hdrPass);
	// TestSpheresForPBRChart(hdrPass);
	// TestSpheresForPBRChartDeferred();

	return true;
}

bool PBRScene::OnBeginPlay()
{
	return true;
}

/*==============//
//   for test   //
//==============*/
void PBRScene::TestSpheresForORMTexture(HDRRenderPass* hdrPass)
{
	auto sphereMesh = RESOURCE.Get<StaticMesh>("Sphere");
	auto baseMat = RESOURCE.Get<Material>("rusted_iron");

	const int rows = 7;
	const int cols = 7;
	const float spacing = 1.4f;

	for (int row = 0; row < rows; ++row)
	{
		// Metallic Factor (0.0 ~ 1.0)
		float metallicFactor = (float)row / (float)(rows - 1);
		float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

		for (int col = 0; col < cols; ++col)
		{
			// Roughness Factor (0.05 ~ 1.0)
			float roughnessFactor = (float)col / (float)(cols - 1);
			roughnessFactor = glm::clamp(roughnessFactor, 0.05f, 1.0f);

			float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

			auto sphereObj = GameObject::Create();
			sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

			// [중요] 각 구체마다 개별적인 파라미터를 가져야 하므로 새 머티리얼 생성
			MaterialPtr pbrMat = Material::Create();

			// 1. 텍스처 공유 (Texture Pointer Copy)
			// 이미지를 새로 로드하는게 아니라, baseMat이 들고 있는 텍스처 포인터만 복사합니다.
			// 따라서 메모리 낭비 없이 텍스처를 재사용합니다.
			if (baseMat)
			{
				pbrMat->diffuse = baseMat->diffuse;
				pbrMat->normal = baseMat->normal;
				pbrMat->orm = baseMat->orm; // ORM 텍스처 연결
			}

			// 2. Factor 값 설정 (Grid Logic)
			// 셰이더 연산: FinalValue = TextureValue * FactorValue
			pbrMat->albedoFactor = glm::vec4(1.0f); // 원본 텍스처 색상 그대로 (Tint 없음)
			pbrMat->metallicFactor = metallicFactor;  // 행에 따라 금속성 조절
			pbrMat->roughnessFactor = roughnessFactor; // 열에 따라 거칠기 조절

			// 렌더러 등록
			auto mr = StaticMeshRenderer::Create(sphereMesh, pbrMat);
			hdrPass->AddRenderer(mr.get());
			sphereObj->AddComponent(std::move(mr));
			AddGameObject(std::move(sphereObj));
		}
	}
}

void PBRScene::TestSpheresForPBRChart(HDRRenderPass* hdrPass)
{
	auto sphereMesh = RESOURCE.Get<StaticMesh>("Sphere");
	TexturePtr sharedWhite = Texture::GetWhiteTexture();

	const int rows = 7;
	const int cols = 7;
	const float spacing = 1.4f;

	for (int row = 0; row < rows; ++row)
	{
		// Metallic (0.0 ~ 1.0)
		float metallicValue = (float)row / (float)(rows - 1);
		float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

		for (int col = 0; col < cols; ++col)
		{
			// Roughness (0.05 ~ 1.0)
			float roughnessValue = (float)col / (float)(cols - 1);
			roughnessValue = glm::clamp(roughnessValue, 0.05f, 1.0f);

			float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

			auto sphereObj = GameObject::Create();
			sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

			// 머티리얼 생성
			MaterialPtr pbrMat = Material::Create();

			// 1. 텍스처는 모두 White로 통일 (공유)
			pbrMat->diffuse = sharedWhite;
			pbrMat->metallic = sharedWhite;
			pbrMat->roughness = sharedWhite;
			pbrMat->ao = sharedWhite;
			// Normal이 없으면 SetToProgram에서 자동으로 Blue(Flat)가 바인딩되므로 생략 가능

			// 2. Factor에 값을 설정 [핵심!]
			pbrMat->albedoFactor = glm::vec4(0.6f, 0.3f, 0.2f, 1.0f); // 빨간공
			pbrMat->metallicFactor = metallicValue;   // 팩터로 조절
			pbrMat->roughnessFactor = roughnessValue; // 팩터로 조절

			// 렌더러 등록
			auto mr = StaticMeshRenderer::Create(sphereMesh, pbrMat);
			mr->SetRenderStage(RenderStage::Forward);
			hdrPass->AddRenderer(mr.get());
			sphereObj->AddComponent(std::move(mr));
			AddGameObject(std::move(sphereObj));
		}
	}
}

void PBRScene::TestSpheresForPBRChartDeferred()
{
	auto sphereMesh = RESOURCE.Get<StaticMesh>("Sphere");
	TexturePtr sharedWhite = Texture::GetWhiteTexture();

	const int rows = 7;
	const int cols = 7;
	const float spacing = 1.4f;

	for (int row = 0; row < rows; ++row)
	{
		// Metallic (0.0 ~ 1.0)
		float metallicValue = (float)row / (float)(rows - 1);
		float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

		for (int col = 0; col < cols; ++col)
		{
			// Roughness (0.05 ~ 1.0)
			float roughnessValue = (float)col / (float)(cols - 1);
			roughnessValue = glm::clamp(roughnessValue, 0.05f, 1.0f);

			float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

			auto sphereObj = GameObject::Create();
			sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

			// 머티리얼 생성
			MaterialPtr pbrMat = Material::Create();

			// 1. 텍스처는 모두 White로 통일 (공유)
			pbrMat->diffuse = sharedWhite;
			pbrMat->metallic = sharedWhite;
			pbrMat->roughness = sharedWhite;
			pbrMat->ao = sharedWhite;
			// Normal이 없으면 SetToProgram에서 자동으로 Blue(Flat)가 바인딩되므로 생략 가능

			// 2. Factor에 값을 설정 [핵심!]
			pbrMat->albedoFactor = glm::vec4(0.025f, 0.025f, 0.025f, 1.0f); // 색상
			pbrMat->metallicFactor = metallicValue;   // 팩터로 조절
			pbrMat->roughnessFactor = roughnessValue; // 팩터로 조절

			// 렌더러 등록
			auto mr = StaticMeshRenderer::Create(sphereMesh, pbrMat);
			sphereObj->AddComponent(std::move(mr));
			AddGameObject(std::move(sphereObj));
		}
	}
}
