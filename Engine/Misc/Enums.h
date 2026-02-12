#pragma once

enum class ResourceType
{
	Animation,
	AudioClip,
	CubeTexture,
	Image,
	Material,
	Model,
	Texture,
	Font,
	StaticMesh,
	SkinnedMesh,
	ScreenMesh,
	InstancedMesh,
	UIMesh,
	DebugMesh,
	
	GraphicsProgram,
	ComputeProgram,

	EnvironmentMap
};

enum class ComponentType
{
	// Core
	Transform,
	Script,
	Animator,
	Rigidbody,
	Camera,

	// Rendering
	SkyLight,
	MeshRenderer,
	SkinnedMeshRenderer,
	StaticMeshRenderer,
	InstancedMeshRenderer,
	MeshOutline,

	// Light
	Light,
	DirectionalLight,
	SpotLight,
	PointLight,

	// Audio
	AudioSource,
	AudioListener,

	// UI
	UICanvas,
	UIImage,
	UIText,

	// Collider
	Collider,
	BoxCollider,
	SphereCollider,
	CapsuleCollider,

	// Misc
	Terrain,
	MAX
};

enum class GameObjectState
{
	Uninitialized, // 생성 직후
	Awake,         // Awake() 호출 완료 (데이터 세팅 됨)
	Started,       // Start() 호출 완료 (첫 프레임 로직 실행 됨)
	Dead           // Destroy 예약됨 (곧 메모리 해제)
};

enum class ComponentState // GameObject 상태와 똑같이.
{
	Uninitialized,
	Awake,
	Started,
	Dead
};

enum class SceneState
{
	Uninitialized,  // 생성 직후, 리소스 로드 전
	Loading,        // (옵션) LoadSceneResources 진행 중
	Awake,          // 객체 배치(OnPlaceActors) 완료, 씬 내부 Awake 완료
	Running,        // Start 완료, 메인 루프(Update) 돌아가는 중
	Paused,         // (옵션) 일시 정지
	Unloading       // 씬 종료/전환 중
};