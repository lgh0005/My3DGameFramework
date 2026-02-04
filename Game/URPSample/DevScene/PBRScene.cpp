#include "EnginePch.h"
#include "PBRScene.h"

#include "Object/GameObject.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Textures/TextureUtils.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Resources/Image.h"
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
#include "SRPSample/Scripts/CameraController.h"
#include "SRPSample/Scripts/PlayerController.h"

DECLARE_DEFAULTS_IMPL(PBRScene)

PBRSceneUPtr PBRScene::Create()
{
	return PBRSceneUPtr(new PBRScene());
}

bool PBRScene::LoadSceneResources()
{
	// 단색 머티리얼 (이미지 생성은 경로 무관하므로 그대로 유지)
	{
		auto solidColorMat = Material::Create();
		if (!solidColorMat) return false;
		solidColorMat->diffuse = TextureUtils::LoadTextureFromImage(Image::CreateSingleColorImage
		(4, 4, glm::vec4(0.75f, 0.1f, 0.2f, 1.0f)).get());
		solidColorMat->metallic = TextureUtils::LoadTextureFromImage(Image::CreateSingleColorImage
		(4, 4, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f)).get());
		solidColorMat->roughness = TextureUtils::LoadTextureFromImage(Image::CreateSingleColorImage
		(4, 4, glm::vec4(0.25f, 0.35f, 0.25f, 1.0f)).get());
		RESOURCE.AddResource<Material>(std::move(solidColorMat), "solidColor");
	}

	// 쇠공 머티리얼 #1
	{
		auto rustedIronMat = Material::Create();
		if (!rustedIronMat) return false;

		// [수정] FILE_MGR.Resolve 사용 (@Game/Images 경로)
		rustedIronMat->diffuse = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_basecolor.png")).get());

		rustedIronMat->roughness = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_roughness.png")).get());

		rustedIronMat->metallic = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_metallic.png")).get());

		rustedIronMat->normal = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_normal.png")).get());

		RESOURCE.AddResource<Material>(std::move(rustedIronMat), "Rusted_Iron");
	}

	// 쇠공 머티리얼 #2
	{
		auto rustedIronMat = Material::Create();
		if (!rustedIronMat) return false;

		rustedIronMat->diffuse = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_basecolor.png")).get());

		rustedIronMat->normal = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_normal.png")).get());

		rustedIronMat->orm = TextureUtils::LoadTextureFromImage
		(Image::Load(FILE_MGR.Resolve("@Game/Images/rustediron/rustediron2_ORM.png")).get());

		RESOURCE.AddResource<Material>(std::move(rustedIronMat), "Rusted_Iron_orm");
	}

	// HDR 큐브맵 머티리얼
	{
		auto hdrCubeMat = Material::Create();
		if (!hdrCubeMat) return false;

		// [수정] IBL 폴더 경로 수정
		hdrCubeMat->diffuse = TextureUtils::LoadTextureFromHDR
		(Image::LoadHDR(FILE_MGR.Resolve("@Game/Images/IBL/mirrored_hall_4k.hdr")).get());

		RESOURCE.AddResource<Material>(std::move(hdrCubeMat), "hdrCubeMat");
	}

	// HDR Skybox 생성 (IBLUtils 사용)
	{
		auto hdrImage = Image::LoadHDR(FILE_MGR.Resolve("@Game/Images/IBL/mirrored_hall_4k.hdr"));
		auto hdrTex = TextureUtils::LoadTextureFromHDR(hdrImage.get());
		RESOURCE.AddResource<Texture>(std::move(hdrTex), "hdrImage");

		auto envMap = EnvironmentMap::CreateIBL(RESOURCE.GetResource<Texture>("hdrImage"));
		RESOURCE.AddResource<EnvironmentMap>(std::move(envMap), "UnviersalSky");
	}

	// 0-4. 머티리얼 2 (빈 머티리얼)
	{
		auto box1Mat = Material::Create();
		RESOURCE.AddResource<Material>(std::move(box1Mat), "GroundMat");
	}

	// 머티리얼 6 (이미 로드된 리소스 참조는 수정 불필요)
	{
		auto box6Mat = Material::Create();
		box6Mat->diffuse = RESOURCE.GetResource<Texture>("toybox_diffuse");
		box6Mat->normal = RESOURCE.GetResource<Texture>("toybox_normal");
		box6Mat->height = RESOURCE.GetResource<Texture>("toybox_disp");
		box6Mat->shininess = 14.0f;
		box6Mat->emissionStrength = 0.0f;
		box6Mat->heightScale = 0.065f;
		RESOURCE.AddResource<Material>(std::move(box6Mat), "boxMat5");
	}

	return true;
}

bool PBRScene::CreateCustomRenderPasses()
{
	// 간단한 머티리얼 포워드 렌더 패스
	{
		// [수정] 셰이더 경로 Resolve
		// Forward_PBR은 샘플 전용 셰이더로 보이므로 @Game/Shaders 경로 사용
		// (만약 엔진 공용 셰이더라면 @Shader로 변경하세요)
		std::string vsPath = FILE_MGR.Resolve("@Game/Shaders/Forward_PBR.vert");
		std::string fsPath = FILE_MGR.Resolve("@Game/Shaders/Forward_PBR.frag");

		auto prog = GraphicsProgram::Create(vsPath, fsPath);
		if (!prog) return false;

		AddRenderPass("simpleHDR", HDRRenderPass::Create(std::move(prog)));
	}

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
		auto cameraCtrl = CameraController::Create();
		cameraObj->AddComponent(std::move(cameraCtrl));

		AddGameObject(std::move(cameraObj));
	}

	// 0. 하늘 GameObject 생성
	{
		auto skyObj = GameObject::Create(); if (!skyObj) return false;
		auto sky = SkyLight::Create(RESOURCE.GetResource<EnvironmentMap>("UnviersalSky"));
		skyObj->AddComponent(std::move(sky));
		AddGameObject(std::move(skyObj));
	}

	// 2. 조명 추가
	{
		// 2. 조명 추가 1
		{
			auto lightGo = GameObject::Create();
			lightGo->SetName("PointLight1");
			auto lightComp = PointLight::Create();
			lightGo->GetTransform().SetPosition(glm::vec3(5.0f, 5.0f, 4.0f));
			lightGo->GetTransform().SetScale(glm::vec3(0.2f));
			lightGo->AddComponent(std::move(lightComp));
			AddGameObject(std::move(lightGo));
		}

		// 2. 조명 추가 2
		{
			auto lightGo = GameObject::Create();
			lightGo->SetName("PointLight2");
			auto lightComp = PointLight::Create();
			lightGo->GetTransform().SetPosition(glm::vec3(-4.0f, 5.0f, 5.0f));
			lightGo->GetTransform().SetScale(glm::vec3(0.2f));
			lightGo->AddComponent(std::move(lightComp));
			AddGameObject(std::move(lightGo));
		}

		// 2. 조명 추가 3
		{
			auto lightGo = GameObject::Create();
			lightGo->SetName("PointLight3");
			auto lightComp = PointLight::Create();
			lightGo->GetTransform().SetPosition(glm::vec3(-4.0f, -6.0f, 6.0f));
			lightGo->GetTransform().SetScale(glm::vec3(0.2f));
			lightGo->AddComponent(std::move(lightComp));
			AddGameObject(std::move(lightGo));
		}

		// 2. 조명 추가 4
		{
			auto lightGo = GameObject::Create();
			lightGo->SetName("PointLight4");
			auto lightComp = PointLight::Create();
			lightGo->GetTransform().SetPosition(glm::vec3(5.0f, -6.0f, 7.0f));
			lightGo->GetTransform().SetScale(glm::vec3(0.2f));
			lightGo->AddComponent(std::move(lightComp));
			AddGameObject(std::move(lightGo));
		}
	}

	// 3. 그림자가 있는 조명 추가
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
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("solidColor"));
		lightGo->AddComponent(std::move(renderer));

		AddGameObject(std::move(lightGo));
	}

	// 6. 큐브 생성 #3
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Ground");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
		cubeTransform.SetScale(glm::vec3(40.0f, 1.0f, 40.0f));

		auto collider = BoxCollider::Create(glm::vec3(40.0f, 1.0f, 40.0f));
		cubeObj->AddComponent(std::move(collider));

		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Static); // 절대 움직이지 않음 (질량 무한대 취급)
		rb->SetRestitution(0.5f);					 // 약간 튕기게 설정 (선택)
		rb->SetFriction(0.5f);
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("GroundMat"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}


	// 6. 패럴랙스 매핑 블록
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("ToyBlock");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(-3.0f, 3.0f, 1.0f));
		cubeTransform.SetRotation(glm::vec3(45.0f));
		cubeTransform.SetScale(glm::vec3(0.75f));

		auto collider = BoxCollider::Create(glm::vec3(0.75f));
		cubeObj->AddComponent(std::move(collider));

		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Static); // 절대 움직이지 않음 (질량 무한대 취급)
		rb->SetRestitution(0.5f);					 // 약간 튕기게 설정 (선택)
		rb->SetFriction(0.5f);
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat5"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 7. 모델
	{
		// 1. 리소스 확보
		auto model = RESOURCE.GetResource<Model>("aliensoldier");
		auto anim1 = RESOURCE.GetResource<Animation>("Idle");
		auto anim2 = RESOURCE.GetResource<Animation>("Walk");

		// 2. AnimController 생성 및 설정
		auto animCtrl = AnimController::Create();
		animCtrl->AddState("Idle", anim1);
		animCtrl->AddState("Walk", anim2);
		animCtrl->SetTransitionDuration("Idle", "Walk", 0.2f);
		animCtrl->SetTransitionDuration("Walk", "Idle", 0.2f);
		animCtrl->SetStartState("Idle");

		// 3. Animator 컴포넌트 먼저 생성 (의존성 주입을 위해)
		// 주의: animator UPtr은 나중에 AddComponent로 소유권이 넘어가므로,
		// Instantiate에 넘겨줄 Raw Pointer를 미리 따놓습니다.
		auto animator = Animator::Create(model, std::move(animCtrl));

		// 4. 콜라이더
		// 0.75f, 4.5f, 0.75f
		auto boxCollider = CapsuleCollider::Create(0.35f, 4.5f);
		boxCollider->SetTrigger(false); // INFO : true 시 Trigger 역할을 하게 된다.
		boxCollider->SetOffset(glm::vec3(0.0f, 2.25f, 0.0f));

		// 5. 리지드 바디
		auto rigidBody = Rigidbody::Create();
		rigidBody->SetMotionType(JPH::EMotionType::Dynamic);
		rigidBody->SetUseGravity(true);
		rigidBody->SetMass(76.0f);
		rigidBody->FreezeRotation(true, true, true);
		rigidBody->SetFriction(0.5f);

		// 6. PlayerController
		auto playerctrl = PlayerController::Create();

		// 4. 모델 인스턴스화 (씬에 등록 + 렌더러 생성 + Animator 주입)
		// Instantiate 내부에서 SkinnedMeshRenderer들이 animatorPtr를 참조하게 됩니다.
		GameObjectUPtr rootUPtr = model->Instantiate(this, animator.get());
		GameObject* rootGO = rootUPtr.get();
		if (rootGO)
		{
			// 5. Root GameObject 설정
			rootGO->SetName("Soldier");
			rootGO->GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -2.0f));
			rootGO->GetTransform().SetScale(glm::vec3(0.025f)); // 모델 단위에 따라 조절 필요

			// 6. 핵심 컴포넌트 부착
			rootGO->AddComponent(std::move(animator));
			rootGO->AddComponent(std::move(playerctrl));
			rootGO->AddComponent(std::move(boxCollider));
			rootGO->AddComponent(std::move(rigidBody));
		}

		// 이제 씬에 등록합니다!
		AddGameObject(std::move(rootUPtr));
	}

		// 3. 구 49개 (ORM 텍스쳐 테스트)
	// TestSpheresForORMTexture(hdrPass);
	// TestSpheresForPBRChart(hdrPass);
	TestSpheresForPBRChartDeferred();

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
	auto sphereMesh = RESOURCE.GetResource<StaticMesh>("Sphere");

	// [핵심] 미리 로드해둔 'Rusted_Iron_orm' 머티리얼을 가져옵니다.
	// 이 머티리얼 안에는 이미 Diffuse, Normal, ORM 텍스처가 들어있습니다.
	auto baseMat = RESOURCE.GetResource<Material>("Rusted_Iron_orm");

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
	auto sphereMesh = RESOURCE.GetResource<StaticMesh>("Sphere");

	// [최적화] 모든 구가 공유할 기본 텍스처들 (White)
	// 텍스처 값(1.0) * 팩터 값(설정값) = 최종 값
	TexturePtr sharedWhite = TextureUtils::GetWhiteTexture();

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
	auto sphereMesh = RESOURCE.GetResource<StaticMesh>("Sphere");

	// [최적화] 모든 구가 공유할 기본 텍스처들 (White)
	// 텍스처 값(1.0) * 팩터 값(설정값) = 최종 값
	TexturePtr sharedWhite = TextureUtils::GetWhiteTexture();

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
