#include "EnginePch.h"
#include "PlayerController.h"
#include "Core/GameObject.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Resources/AnimController.h"

PlayerController::PlayerController() = default;
PlayerController::~PlayerController() = default;

PlayerControllerUPtr PlayerController::Create()
{
    auto script = PlayerControllerUPtr(new PlayerController());
    return std::move(script);
}

void PlayerController::Start()
{
    INPUT_MGR.MapAction("Player_Up", GLFW_KEY_UP);
    INPUT_MGR.MapAction("Player_Down", GLFW_KEY_DOWN);
    INPUT_MGR.MapAction("Player_Left", GLFW_KEY_LEFT);
    INPUT_MGR.MapAction("Player_Right", GLFW_KEY_RIGHT);

    m_animator = GetOwner()->GetComponent<Animator>();
    m_animController = m_animator->GetController();
    if (!m_animator || !m_animController) return;
}

void PlayerController::Update()
{
    HandleMovement(TIME.GetDeltaTime());
    UpdateFiniteStateMachine();
}

void PlayerController::HandleMovement(float dt)
{
    auto& transform = GetTransform();
    float speed = m_moveSpeed * dt;
    m_isMoving = false;

    // 1. 화살표 입력 처리
    if (INPUT_MGR.GetButton("Player_Up"))
    {
        // 모델 기준 앞(Forward)으로 이동
        transform.Translate(transform.GetForwardVector() * speed);
        m_isMoving = true;
    }
    if (INPUT_MGR.GetButton("Player_Down"))
    {
        transform.Translate(transform.GetForwardVector() * -speed);
        m_isMoving = true;
    }
    // 좌우 이동 시 모델이 회전하지 않는다면 그냥 옆으로 이동 (게걸음)
    if (INPUT_MGR.GetButton("Player_Right"))
    {
        transform.Translate(transform.GetRightVector() * speed);
        m_isMoving = true;
    }
    if (INPUT_MGR.GetButton("Player_Left"))
    {
        transform.Translate(transform.GetRightVector() * -speed);
        m_isMoving = true;
    }
}

void PlayerController::UpdateFiniteStateMachine()
{
    if (m_isMoving) m_animController->CrossFade("Walk");
    else m_animController->CrossFade("Idle");
}
