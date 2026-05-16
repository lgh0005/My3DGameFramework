#include "GamePch.h"
#include "PlayerController.h"

namespace MGF3D
{
	PlayerController::PlayerController(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) {}
	PlayerController::~PlayerController() = default;
	PlayerController::PlayerController(PlayerController&& other) noexcept = default;
	PlayerController& PlayerController::operator=(PlayerController&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 PlayerController::s_typeIndex = -1;
	const MGFType* PlayerController::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Script");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void PlayerController::Awake()
	{
		// 1. 키보드 액션 매핑
		Inputs::MapKeyboard("PlayerForward", GLFW_KEY_UP);
		Inputs::MapKeyboard("PlayerBackward", GLFW_KEY_DOWN);
		Inputs::MapKeyboard("PlayerLeft", GLFW_KEY_LEFT);
		Inputs::MapKeyboard("PlayerRight", GLFW_KEY_RIGHT);

		// 2. AnimController 애니메이션 상태 전환
		auto animator = Entities::GetComponent<Animator>(GetOwnerID());
		auto animController = animator->GetController();
		auto idleAnim = MGF_ASSET.GetAsset<Animation>("@GameAsset/Models/aliensoldier/Idle.myanim");
		auto walkAnim = MGF_ASSET.GetAsset<Animation>("@GameAsset/Models/aliensoldier/Walking.myanim");
		animController->AddState("Idle", idleAnim, true, 1.0f);
		animController->AddState("Walk", walkAnim, true, 1.0f);

		animController->SetTransitionDuration(StringHash("Idle"), StringHash("Walk"), 0.2f);
		animController->SetTransitionDuration(StringHash("Walk"), StringHash("Idle"), 0.25f);

		animController->SetStartState(StringHash("Idle"));
		animController->Play(StringHash("Idle"));
	}

	void PlayerController::Update()
	{
		float dt = MGF_TIME.GetDeltaTime();
		ProcessKeyboard(dt);
	}

	void PlayerController::ProcessKeyboard(float dt)
	{
		auto transform = Entities::GetComponent<Transform>(GetOwnerID());
		if (!transform) return;

		// 1. 이동 로직
		vec3 velocity = vec3(0.0f);
		if (Inputs::GetButton("PlayerForward"))  velocity -= transform->GetForward();
		if (Inputs::GetButton("PlayerBackward")) velocity += transform->GetForward();
		if (Inputs::GetButton("PlayerRight"))     velocity -= transform->GetRight();
		if (Inputs::GetButton("PlayerLeft"))    velocity += transform->GetRight();

		m_isMoving = Math::HasLength(velocity);
		if (m_isMoving)
			transform->Translate(Math::Normalize(velocity) * m_moveSpeed * dt);

		// 2. 애니메이션 전환
		ChangeAnimation();
	}

	void PlayerController::ChangeAnimation()
	{
		auto animator = Entities::GetComponent<Animator>(GetOwnerID());
		auto animController = animator->GetController();

		if (m_isMoving) animController->CrossFade(StringHash("Walk"));
		else animController->CrossFade(StringHash("Idle"));
	}
}