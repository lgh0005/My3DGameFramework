#include "pch.h"
#include "DevScene.h"

#include "GameObject/GameObject.h"
#include "Graphics/Renderer.h"
#include "Resources/Program.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Resources/Image.h"
#include "Resources/Animation.h"
#include "Resources/Model.h"
#include "Graphics/Buffer.h"
#include "Resources/InstancedMesh.h"
#include "Graphics/Geometry.h"
#include "Graphics/SkyLight.h"
#include "Resources/AudioClip.h"
#include "Resources/AnimController.h"
#include "Resources/EnvironmentMap.h"

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/InstancedMeshRenderer.h"
#include "Components/SpotLight.h"
#include "Components/DirectionalLight.h"
#include "Components/PointLight.h"
#include "Components/Animator.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"
#include "Components/MeshOutline.h"
#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/CapsuleCollider.h"

#include "SRPSample/RenderPasses/InstancedRenderPass.h"
#include "SRPSample/RenderPasses/SimpleRenderPass.h"
#include "SRPSample/RenderPasses/EnvironmentRenderPass.h"

#include "SRPSample/Scripts/CameraController.h"
#include "SRPSample/Scripts/PlayerController.h"

DECLARE_DEFAULTS_IMPL(DevScene)

DevSceneUPtr DevScene::Create()
{
	return DevSceneUPtr(new DevScene());
}

bool DevScene::LoadSceneResources()
{
	// 0-3. 머티리얼 1
	{
		auto lightMat = Material::Create();
		lightMat->shininess = 16.0f;
		lightMat->emissionStrength = 0.0f;
		lightMat->heightScale = 0.0f;
		RESOURCE.AddResource<Material>(std::move(lightMat), "LightMat");
	}

	// 0-4. 머티리얼 2
	{
		auto box1Mat = Material::Create();
		box1Mat->diffuse = RESOURCE.GetResource<Texture>("container");
		box1Mat->emission = RESOURCE.GetResource<Texture>("matrix");
		box1Mat->shininess = 16.0f;
		box1Mat->emissionStrength = 5.0f;
		box1Mat->heightScale = 0.0f;
		RESOURCE.AddResource<Material>(std::move(box1Mat), "boxMat1");
	}

	// 0-5. 머티리얼 3
	{
		auto box2Mat = Material::Create();
		box2Mat->diffuse = RESOURCE.GetResource<Texture>("container2");
		box2Mat->specular = RESOURCE.GetResource<Texture>("container2_specular");
		box2Mat->emission = RESOURCE.GetResource<Texture>("matrix");
		box2Mat->shininess = 16.0f;
		box2Mat->emissionStrength = 2.0f;
		box2Mat->heightScale = 0.0f;
		RESOURCE.AddResource<Material>(std::move(box2Mat), "boxMat2");
	}

	// 0-6. 머티리얼 4
	{
		auto box4Mat = Material::Create();
		box4Mat->diffuse = RESOURCE.GetResource<Texture>("marble");
		box4Mat->shininess = 20.0f;
		box4Mat->emissionStrength = 0.0f;
		box4Mat->heightScale = 0.0f;
		RESOURCE.AddResource<Material>(std::move(box4Mat), "boxMat3");
	}

	// 머티리얼 5
	{
		auto box5Mat = Material::Create();
		box5Mat->diffuse = RESOURCE.GetResource<Texture>("brickwall");
		box5Mat->normal = RESOURCE.GetResource<Texture>("brickwall_normal");
		box5Mat->shininess = 64.0f;
		box5Mat->emissionStrength = 0.0f;
		box5Mat->heightScale = 0.0f;
		RESOURCE.AddResource<Material>(std::move(box5Mat), "boxMat4");
	}

	// 머티리얼 6
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

	// 0-7. 풀떼기
	{
		auto grassMat = Material::Create();
		grassMat->diffuse = RESOURCE.GetResource<Texture>("grass");
		grassMat->emission = nullptr;
		grassMat->emissionStrength = 0.0f;
		RESOURCE.AddResource<Material>(std::move(grassMat), "grassMat");
	}

	// 8. 하늘 환경맵
	{
		RESOURCE.AddResource<EnvironmentMap>
		(
			EnvironmentMap::Create(RESOURCE.GetResource<CubeTexture>("SkyboxTexture")), 
			"StandardSkybox"
		);
	}

	return true;
}

bool DevScene::CreateCustomRenderPasses()
{
	auto pipeline = RENDER.GetRenderer()->GetPipeline();

	// 1. Instanced 셰이더 (잔디)
	{
		auto prog = Program::Create(
			"./Resources/Shaders/grass.vert",
			"./Resources/Shaders/grass.frag");
		if (!prog) return false;
		AddRenderPass("Instanced", InstancedRenderPass::Create(std::move(prog)));
	}

	// 2. Simple 셰이더 (조명 기즈모)
	{
		auto prog = Program::Create(
			"./Resources/Shaders/simple.vert",
			"./Resources/Shaders/simple.frag");
		if (!prog) return false;
		AddRenderPass("LightGizmo", SimpleRenderPass::Create(std::move(prog)));
	}

	// 3. 환경맵
	{
		auto prog = Program::Create
		(
			"./Resources/Shaders/environment.vert",
			"./Resources/Shaders/environment.frag"
		);
		if (!prog) return false;
		CubeTexturePtr cubeTex = RESOURCE.GetResource<CubeTexture>("SkyboxTexture");
		if (!cubeTex) return false;
		AddRenderPass("EnvMap", EnvironmentRenderPass::Create(std::move(prog), cubeTex));
	}

	return true;
}

bool DevScene::SetupSceneEnvironment()
{
	auto sky = SkyLight::Create(RESOURCE.GetResource<EnvironmentMap>("StandardSkybox"));
	if (!sky) return false;
	SetSkyLight(std::move(sky));
	return true;
}

bool DevScene::OnPlaceActors()
{
	SimpleRenderPass* lightPass = (SimpleRenderPass*)GetRenderPass("LightGizmo");
	InstancedRenderPass* grassPass = (InstancedRenderPass*)GetRenderPass("Instanced");
	EnvironmentRenderPass* envMapPass = (EnvironmentRenderPass*)GetRenderPass("EnvMap");

	// 1. 카메라 GameObject 생성
	{
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

		// 오디오 리스너 생성 및 추가
		auto audioListener = AudioListener::Create();
		cameraObj->AddComponent(std::move(audioListener));

		AddGameObject(std::move(cameraObj));
	}

	// 조명 큐브 #1
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("SpotLight");

		auto lightComp = SpotLight::Create();
		lightComp->SetCastShadow(true);
		lightGo->GetTransform().SetPosition(glm::vec3(1.0f, 4.0f, 4.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightComp->SetCutoff(glm::vec2(60.0, 5.0f));
		lightComp->SetDistance(128.0f);
		lightGo->AddComponent(std::move(lightComp));

		auto renderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("LightMat"));
		renderer->SetRenderStage(RenderStage::Forward);
		lightPass->AddRenderer(renderer.get());

		lightGo->AddComponent(std::move(renderer));
		AddGameObject(std::move(lightGo));
	}

	// 조명 큐브 #2
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("PointLight");

		auto lightComp = PointLight::Create();
		lightGo->GetTransform().SetPosition(glm::vec3(5.0f, 3.0f, 7.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightGo->AddComponent(std::move(lightComp));

		auto renderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("LightMat"));
		renderer->SetRenderStage(RenderStage::Forward);
		lightPass->AddRenderer(renderer.get());

		lightGo->AddComponent(std::move(renderer));
		AddGameObject(std::move(lightGo));
	}

	// 4. 큐브 생성 #1
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box1");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(3.0f, 2.75f, -4.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat1"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 5. 큐브 생성 #2
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box2");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, 0.75f, 2.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 20.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat2"));
		cubeObj->AddComponent(std::move(meshRenderer));

		// TEMP : BGM 재생
		// TODO : 이걸 OnBeginPlay에서 실행시킬 방안을 찾아야 함
		auto audiSourc = AudioSource::Create(RESOURCE.GetResource<AudioClip>("MyBGM"));
		audiSourc->Play();
		cubeObj->AddComponent(std::move(audiSourc));

		AddGameObject(std::move(cubeObj));
	}

	// 5. 큐브 생성 #3
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box2");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(-1.0f, 1.75f, -4.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		// Outline Component
		auto outline = MeshOutline::Create(glm::vec3(1.0f, 1.0f, 0.5f), 1.0f);
		cubeObj->AddComponent(std::move(outline));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat4"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 5. 큐브 생성 #4
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box2");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(-1.0f, 1.05f, -9.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 70.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(3.0f, 3.0f, 3.0f));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat5"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 6. 큐브 생성 #3
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Ground");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
		cubeTransform.SetScale(glm::vec3(100.0f, 1.0f, 100.0f));

		auto collider = BoxCollider::Create(glm::vec3(100.0f, 1.0f, 100.0f));
		cubeObj->AddComponent(std::move(collider));

		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Static); // 절대 움직이지 않음 (질량 무한대 취급)
		rb->SetRestitution(0.5f);					 // 약간 튕기게 설정 (선택)
		rb->SetFriction(0.5f);
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat3"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 6. 떨어지는 큐브
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("FallingBox");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, 6.0f, 0.0f));
		cubeTransform.SetRotation(glm::vec3(30.0f, 60.0f, 75.0f));
		cubeTransform.SetScale(glm::vec3(0.75f));

		// 3. 콜라이더
		auto collider = BoxCollider::Create(glm::vec3(0.75f));
		cubeObj->AddComponent(std::move(collider));

		// 4. 리지드바디 추가 (DYNAMIC)
		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Dynamic);
		rb->SetUseGravity(true); // 중력 켜기
		rb->SetMass(10.0f);      // 질량 10kg
		rb->SetRestitution(0.3f); // 튕김 계수
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("material_SRP"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 7. 떨어지는 구체
	{
		auto sphereObj = GameObject::Create();
		sphereObj->SetName("FallingBall");
		auto& sphereTransform = sphereObj->GetTransform();
		sphereTransform.SetPosition(glm::vec3(3.0f, 3.0f, 3.0f));
		sphereTransform.SetRotation(glm::vec3(30.0f, 60.0f, 75.0f));
		sphereTransform.SetScale(glm::vec3(0.75f));

		// 3. 콜라이더
		auto collider = SphereCollider::Create(0.75f);
		sphereObj->AddComponent(std::move(collider));

		// 4. 리지드바디 추가 (DYNAMIC)
		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Dynamic);
		rb->SetUseGravity(true); // 중력 켜기
		rb->SetMass(10.0f);      // 질량 10kg
		rb->SetRestitution(0.3f); // 튕김 계수
		sphereObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Sphere"), RESOURCE.GetResource<Material>("material_SRP"));
		sphereObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(sphereObj));
	}

	// 7. 임시 환경맵 큐브
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("EnvCube");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(-2.5f, 0.5f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.0f));

		auto meshRenderer = StaticMeshRenderer::Create
		(
			RESOURCE.GetResource<StaticMesh>("Cube"),
			RESOURCE.GetResource<Material>("boxMat1") // 임시 재질
		);
		meshRenderer->SetRenderStage(RenderStage::Forward);
		envMapPass->AddRenderer(meshRenderer.get());
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 7. 모델 (Alien Soldier - Skinned Mesh)
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

		// 3. Animator 컴포넌트 미리 생성
		// (Instantiate 내부에서 SkinnedMeshRenderer들이 바인딩할 때 필요하므로 포인터 따기)
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

		// 4. Instantiate (이제 한 줄로 끝!)
		// 내부에서 노드 계층 구조 생성 + 자식들 Scene 등록 + 렌더러 부착까지 다 해줍니다.
		GameObjectUPtr rootUPtr = model->Instantiate(this, animator.get());
		GameObject* rootGO = rootUPtr.get();
		if (rootUPtr)
		{
			// 5. Root 설정 (이름, Transform)
			rootGO->SetName("Soldier");
			rootGO->GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -2.0f));
			rootGO->GetTransform().SetScale(glm::vec3(0.025f));
			rootGO->GetTransform().SetRotation(glm::vec3(0.0f, 45.0f, 0.0f));

			// 6. 핵심 컴포넌트 부착
			rootGO->AddComponent(std::move(animator));
			rootGO->AddComponent(std::move(playerctrl));
			rootGO->AddComponent(std::move(boxCollider));
			rootGO->AddComponent(std::move(rigidBody));
		}

		AddGameObject(std::move(rootUPtr));
	}

	// 가방 (Backpack - Static Mesh)
	{
		auto model = RESOURCE.GetResource<Model>("backpack");

		// 1. Instantiate (애니메이터 없으므로 nullptr 전달)
		// StaticMeshRenderer는 Animator 인자가 필요 없으므로 안전함.
		GameObjectUPtr rootUPtr = model->Instantiate(this);

		if (rootUPtr)
		{
			GameObject* rootGO = rootUPtr.get();

			// 2. Root 설정
			rootGO->SetName("Backpack");
			rootGO->GetTransform().SetPosition(glm::vec3(6.0f, 2.0f, -6.0f));
			rootGO->GetTransform().SetScale(glm::vec3(0.0125f));

			// 3. 추가 컴포넌트가 없다면 바로 입주 신고
			AddGameObject(std::move(rootUPtr));
		}
	}

	// 8. 벽
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("StaticWallBox");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(20.0f, 0.0f, 20.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 45.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(10.0f, 20.0f, 2.5f));

		auto collider = BoxCollider::Create(glm::vec3(10.0f, 20.0f, 2.5f));
		cubeObj->AddComponent(std::move(collider));

		auto outline = MeshOutline::Create();
		cubeObj->AddComponent(std::move(outline));

		auto rb = Rigidbody::Create();
		rb->SetMotionType(JPH::EMotionType::Static); // 절대 움직이지 않음 (질량 무한대 취급)
		rb->SetRestitution(0.5f);					 // 약간 튕기게 설정 (선택)
		rb->SetFriction(0.5f);
		cubeObj->AddComponent(std::move(rb));

		auto meshRenderer = StaticMeshRenderer::Create
		(RESOURCE.GetResource<StaticMesh>("Cube"), RESOURCE.GetResource<Material>("boxMat4"));
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 잔디밭
	// PlantTenThousandGrass(grassPass);

	return true;
}

bool DevScene::OnBeginPlay()
{
	// TODO : 노래 재생
	return true;
}

/*==============//
//   for test   //
//==============*/
void DevScene::PlantTenThousandGrass(InstancedRenderPass* pass)
{
	const int32 grassCount = 10000;
	auto bladeMesh = std::static_pointer_cast<StaticMesh>
		(RESOURCE.GetResource<StaticMesh>("Plane"));
	auto grassMat = RESOURCE.GetResource<Material>("grassMat");
	if (!bladeMesh || !grassMat) return;

	std::vector<glm::vec3> instanceData;
	instanceData.resize(grassCount);
	srand((unsigned int)time(NULL));
	for (int i = 0; i < grassCount; ++i)
	{
		float x = ((float)rand() / (float)RAND_MAX * 10.0f) - 5.0f; // -5 ~ +5 (바닥 크기)
		float z = ((float)rand() / (float)RAND_MAX * 10.0f) - 5.0f; // -5 ~ +5
		float y_rot = glm::radians((float)rand() / (float)RAND_MAX * 360.0f);
		instanceData[i] = glm::vec3(x, y_rot, z);
	}

	BufferPtr instanceBuffer = Buffer::CreateWithData
	(
		GL_ARRAY_BUFFER, GL_STATIC_DRAW,
		instanceData.data(), sizeof(glm::vec3), instanceData.size()
	);

	auto instancedGrass = InstancedMesh::Create
	(
		bladeMesh,
		instanceBuffer,
		grassCount,
		InstancedMesh::Vec3Layout()
	);
	// instancedGrass->SetMaterial(grassMat);

	auto grassGo = GameObject::Create();
	grassGo->SetName("Grass_Field");
	grassGo->GetTransform().SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));

	// TODO : 설계 방식을 다시 한 번 생각해볼 필요가 있음. 사실상 InstancedMeshRenderer는
	// 그냥 Mesh(InstancedMesh)를 멤버로 들고 있는 저장소 역할에 불과함.
	auto renderer = InstancedMeshRenderer::Create(std::move(instancedGrass), grassMat);
	pass->AddRenderer(renderer.get());
	grassGo->AddComponent(std::move(renderer));
	AddGameObject(std::move(grassGo));
}

void DevScene::ManyCubesForStressTest()
{
	// [Stress Test] 많은 수의 무작위 큐브 생성
	// 목적: CPU Draw Call 병목 현상 확인 (약 5,000~6,000 Draw Calls 발생 예상)
	// INFO : 컬링 없이 순수 렌더링으로 2500개 정도면 28ms 정도가 소요
	// INFO : 컬링을 수행하면 14ms로 최적화
	// INFO : 컬링 없이 순수 렌더링으로 1000개 정도면 14ms 정도가 소요
	// INFO : 컬링을 수행하면 8ms로 최적화
	// INFO : frstum의 margin 값에 영향을 받을 수 있음. 적정값은 2.0f에서 5.0f 정도?
	// margin값을 너무 많이 주면 컬링이 잘 안되고 너무 적게 주면 그림자가 비정상적으로
	// 없어지니(컬링이 엄격히 되니까) 적정값을 찾아서 수정 필요. (Frustum.h 참고)
	const int stressCount = 2500;
	const float range = 40.0f; // 배치를 퍼뜨릴 범위 (-40 ~ +40)

	auto cubeMesh = RESOURCE.GetResource<StaticMesh>("Cube");
	auto cubeMat = RESOURCE.GetResource<Material>("boxMat1");

	if (cubeMesh && cubeMat)
	{
		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		for (int i = 0; i < stressCount; ++i)
		{
			auto cubeObj = GameObject::Create();

			// 1. 위치 및 회전 무작위 설정
			float x = ((float)std::rand() / (float)RAND_MAX * range * 2.0f) - range;
			float z = ((float)std::rand() / (float)RAND_MAX * range * 2.0f) - range;
			float y = ((float)std::rand() / (float)RAND_MAX * 5.0f) + 1.0f; // 높이 1.0 ~ 6.0 공중 부양

			float rotX = (float)(std::rand() % 360);
			float rotY = (float)(std::rand() % 360);

			auto& transform = cubeObj->GetTransform();
			transform.SetPosition(glm::vec3(x, y, z));
			transform.SetRotation(glm::vec3(rotX, rotY, 0.0f));
			transform.SetScale(glm::vec3(0.5f)); // 크기는 0.5배로

			// 2. MeshRenderer 컴포넌트 추가
			// 중요: 인스턴싱이 아니라 개별 렌더러를 생성하여 부착함
			auto meshRenderer = StaticMeshRenderer::Create(cubeMesh, cubeMat);
			cubeObj->AddComponent(std::move(meshRenderer));

			// 3. 씬에 등록
			AddGameObject(std::move(cubeObj));
		}
	}
}

void DevScene::FallRandomCubes()
{
	// 7. 랜덤 큐브 20개 드랍!
	{
		// 랜덤 엔진 초기화
		std::random_device rd;
		std::mt19937 gen(rd());

		// 랜덤 범위 설정
		std::uniform_real_distribution<float> distPosX(-25.0f, 25.0f); // X축 범위 (폭 50)
		std::uniform_real_distribution<float> distPosZ(-25.0f, 25.0f); // Z축 범위 (폭 50)
		std::uniform_real_distribution<float> distPosY(10.0f, 30.0f);  // Y축 높이 (10 ~ 30m 상공)
		std::uniform_real_distribution<float> distRot(0.0f, 360.0f);   // 회전 각도 (0 ~ 360도)

		for (int i = 0; i < 20; i++)
		{
			auto cubeObj = GameObject::Create();
			cubeObj->SetName("FallingBox_" + std::to_string(i)); // 이름 구분: FallingBox_0, FallingBox_1...

			auto& cubeTransform = cubeObj->GetTransform();

			// 1. 위치 및 회전 랜덤 설정
			cubeTransform.SetPosition(glm::vec3(distPosX(gen), distPosY(gen), distPosZ(gen)));
			cubeTransform.SetRotation(glm::vec3(distRot(gen), distRot(gen), distRot(gen)));
			cubeTransform.SetScale(glm::vec3(0.75f));

			// 2. [물리] 콜라이더
			auto collider = BoxCollider::Create(glm::vec3(0.75f));
			cubeObj->AddComponent(std::move(collider));

			// 3. [물리] 리지드바디 (Dynamic)
			auto rb = Rigidbody::Create();
			rb->SetMotionType(JPH::EMotionType::Dynamic);
			rb->SetUseGravity(true);
			rb->SetMass(10.0f);
			rb->SetRestitution(0.5f); // 0.5 정도 주면 바닥에 닿을 때 통통 튀어서 더 재밌습니다.
			rb->SetFriction(0.6f);    // 마찰력도 적당히
			cubeObj->AddComponent(std::move(rb));

			// 4. 렌더러
			auto meshRenderer = StaticMeshRenderer::Create
			(
				RESOURCE.GetResource<StaticMesh>("Cube"),
				RESOURCE.GetResource<Material>("material_SRP")
			);
			cubeObj->AddComponent(std::move(meshRenderer));

			// 씬에 등록
			AddGameObject(std::move(cubeObj));
		}
	}
}
