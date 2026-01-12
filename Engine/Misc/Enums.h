#pragma once

enum class ResourceType
{
	Animation,
	AudilClip,
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
	Shader,
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

enum class ComponentState
{
	Uninitialized,  // 생성자 호출 직후 (아직 Awake/Start 안 함)
	Initialized,    // Awake 완료 (데이터 세팅 완료)
	Running,        // Start 완료 (이제부터 정상적으로 Update 돔) -> 기존의 Active 대체
	Dead            // 사망 선고 (메모리 해제 대기)
};

enum class GameObjectState
{
	Uninitialized, // 생성 직후
	Awake,         // Awake 호출 완료
	Active,        // Start 호출 완료 + 활성화 상태
	Inactive,      // 비활성화 상태 (SetActive(false))
	Dead           // Destroy 예약됨 (다음 프레임 삭제)
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