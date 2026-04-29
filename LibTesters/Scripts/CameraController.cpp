#include "GamePch.h"
#include "CameraController.h"
#include "Managers/TypeManager.h"
#include "Managers/TimeManager.h"
#include "Components/Transform.h"
#include "Entities/GameObject.h"

namespace MGF3D
{
	CameraController::CameraController(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) {}
	CameraController::~CameraController() = default;
	CameraController::CameraController(CameraController&& other) noexcept = default;
	CameraController& CameraController::operator=(CameraController&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 CameraController::s_typeIndex = -1;
	const MGFType* CameraController::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Script");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void CameraController::Awake()
	{
		// 1. 키보드 액션 매핑
		Inputs::MapKeyboard("CamForward", GLFW_KEY_W);
		Inputs::MapKeyboard("CamBackward", GLFW_KEY_S);
		Inputs::MapKeyboard("CamLeft", GLFW_KEY_A);
		Inputs::MapKeyboard("CamRight", GLFW_KEY_D);
		Inputs::MapKeyboard("CamUp", GLFW_KEY_E);
		Inputs::MapKeyboard("CamDown", GLFW_KEY_Q);

		// 2. 마우스 액션 매핑
		Inputs::MapMouse("CamRotateMode", GLFW_MOUSE_BUTTON_RIGHT);
	}

	void CameraController::Update()
	{
		float dt = MGF_TIME.GetDeltaTime();
		ProcessKeyboard(dt);
		ProcessMouse(dt);
	}

	void CameraController::ProcessKeyboard(float dt)
	{
		auto transform = Entities::GetComponent<Transform>(GetOwnerID());
		if (!transform) return;

		vec3 velocity = vec3(0.0f);

		if (Inputs::GetButton("CamForward"))  velocity += transform->GetForward();
		if (Inputs::GetButton("CamBackward")) velocity -= transform->GetForward();
		if (Inputs::GetButton("CamLeft"))     velocity -= transform->GetRight();
		if (Inputs::GetButton("CamRight"))    velocity += transform->GetRight();
		if (Inputs::GetButton("CamUp"))       velocity += transform->GetUp();
		if (Inputs::GetButton("CamDown"))     velocity -= transform->GetUp();

		if (Math::HasLength(velocity))
			transform->Translate(Math::Normalize(velocity) * m_moveSpeed * dt);
	}

	void CameraController::ProcessMouse(float dt)
	{
		if (!Inputs::GetButton("CamRotateMode")) return;

		auto transform = Entities::GetComponent<Transform>(GetOwnerID());
		if (!transform) return;

		vec2 mouseDelta = Inputs::GetMouseDelta();
		if (Math::IsNearlyZero(mouseDelta.x) && Math::IsNearlyZero(mouseDelta.y)) return;

		// 3. 도(Degree) 단위로 회전량 누적
		m_yaw -= mouseDelta.x * m_lookSensitivity;
		m_pitch -= mouseDelta.y * m_lookSensitivity;

		// 2. Pitch 제한 (Clamping)
		m_pitch = Math::Clamp(m_pitch, -89.0f, 89.0f);

		// 3. 새로운 회전값 생성
		quat qYaw = Math::AngleAxis(Math::ToRadians(m_yaw), vec3(0.0f, 1.0f, 0.0f));
		quat qPitch = Math::AngleAxis(Math::ToRadians(m_pitch), vec3(1.0f, 0.0f, 0.0f));

		// 최종 회전 적용 (절대적인 회전값 주입)
		transform->SetLocalRotation(qYaw * qPitch);
	}
}