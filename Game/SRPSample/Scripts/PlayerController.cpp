#include "EnginePch.h"
#include "PlayerController.h"
#include "Object/GameObject.h"
#include "Components/Rigidbody.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include "Resources/Animations/AnimController.h"

DECLARE_DEFAULTS_IMPL(PlayerController)

PlayerControllerUPtr PlayerController::Create()
{
    auto script = PlayerControllerUPtr(new PlayerController());
    return std::move(script);
}

void PlayerController::OnStart()
{
    INPUT_MGR.MapAction("Player_Up", GLFW_KEY_UP);
    INPUT_MGR.MapAction("Player_Down", GLFW_KEY_DOWN);
    INPUT_MGR.MapAction("Player_Left", GLFW_KEY_LEFT);
    INPUT_MGR.MapAction("Player_Right", GLFW_KEY_RIGHT);
    INPUT_MGR.MapAction("Disable_Self", GLFW_KEY_F);

    m_rigidbody = GetOwner()->GetComponent<Rigidbody>();
    m_animator = GetOwner()->GetComponent<Animator>();
    m_animController = m_animator->GetController();
    if (!m_rigidbody || !m_animator || !m_animController) return;
}

void PlayerController::OnUpdate()
{
    ActiveSelfTest();
    HandleMovement(TIME.GetDeltaTime());
    UpdateFiniteStateMachine();
}

void PlayerController::OnFixedUpdate()
{
    // 3. 물리 엔진에 속도 적용
    glm::vec3 currentVel = m_rigidbody->GetLinearVelocity();
    glm::vec3 targetVel = m_moveDir * m_moveSpeed;

    // 점프 구현 시 targetVel.y를 건드리면 됨. 지금은 중력 유지.
    targetVel.y = currentVel.y;

    m_rigidbody->SetLinearVelocity(targetVel);
}

void PlayerController::HandleMovement(float dt)
{
    auto& transform = GetTransform();
    m_moveDir = glm::vec3(0.0f);
    m_isMoving = false;

    // 1. 화살표 입력 처리 : 입력에 따른 이동 방향 벡터 계산
    if (INPUT_MGR.GetButton("Player_Up"))    m_moveDir += transform.GetForwardVector();
    if (INPUT_MGR.GetButton("Player_Down"))  m_moveDir -= transform.GetForwardVector();
    if (INPUT_MGR.GetButton("Player_Right")) m_moveDir += transform.GetRightVector();
    if (INPUT_MGR.GetButton("Player_Left"))  m_moveDir -= transform.GetRightVector();

    // 2. 입력이 있으면 정규화
    if (glm::length(m_moveDir) > 0.1f)
    {
        m_moveDir = glm::normalize(m_moveDir);
        m_isMoving = true;
    }
}

void PlayerController::UpdateFiniteStateMachine()
{
    if (m_isMoving) m_animController->CrossFade("Walk");
    else m_animController->CrossFade("Init");
}

void PlayerController::ActiveSelfTest()
{
    if (INPUT_MGR.GetButton("Disable_Self"))
    {
        SceneUtils::Destroy(GetOwner());
        return;
    }
}

void PlayerController::OnCollisionEnter(Collider* other)
{
    LOG_INFO
    (
        "[Script : PlayerController] Collision Occured with {}", 
        other->GetOwner()->GetName()
    );
}