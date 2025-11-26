#include "pch.h"
#include "CameraController.h"
#include "Components/Transform.h"

CameraControllerUPtr CameraController::Create()
{
    auto script = CameraControllerUPtr(new CameraController());
    return std::move(script);
}

void CameraController::Start()
{
    glm::vec3 rotation = GetTransform().GetRotationEuler();
    m_pitch = rotation.x;
    m_yaw = rotation.y;

    INPUT.MapAction("MoveForward", GLFW_KEY_W);
    INPUT.MapAction("MoveBackward", GLFW_KEY_S);
    INPUT.MapAction("MoveLeft", GLFW_KEY_A);
    INPUT.MapAction("MoveRight", GLFW_KEY_D);
    INPUT.MapAction("MoveUp", GLFW_KEY_E);
    INPUT.MapAction("MoveDown", GLFW_KEY_Q);

    INPUT.MapMouseAction("ControlCamera", GLFW_MOUSE_BUTTON_RIGHT);
}

void CameraController::Update()
{
    auto deltaTime = TIME.GetDeltaTime();
    HandleMovement(deltaTime);
    HandleRotation(deltaTime);
}

void CameraController::HandleMovement(float dt)
{
    auto& transform = GetTransform();
    float speed = m_moveSpeed * dt;

    if (INPUT.GetButton("MoveForward"))  
        transform.Translate(transform.GetForwardVector() * speed);
    if (INPUT.GetButton("MoveBackward")) 
        transform.Translate(transform.GetForwardVector() * -speed);

    if (INPUT.GetButton("MoveRight"))    
        transform.Translate(transform.GetRightVector() * speed);
    if (INPUT.GetButton("MoveLeft"))     
        transform.Translate(transform.GetRightVector() * -speed);

    if (INPUT.GetButton("MoveUp"))       
        transform.Translate(transform.GetUpVector() * speed);
    if (INPUT.GetButton("MoveDown"))     
        transform.Translate(transform.GetUpVector() * -speed);
}

void CameraController::HandleRotation(float dt)
{
    if (INPUT.GetButtonDown("ControlCamera"))
    {
        m_controlEnabled = true;
        m_prevMousePos = INPUT.GetMousePos(); // 클릭 순간 튀는 현상 방지
    }
    else if (INPUT.GetButtonUp("ControlCamera"))
    {
        m_controlEnabled = false;
    }
    if (!m_controlEnabled) return;

    // 마우스 델타 계산
    glm::vec2 currentPos = INPUT.GetMousePos();
    glm::vec2 delta = currentPos - m_prevMousePos;
    m_prevMousePos = currentPos;

    // 회전 적용
    m_yaw -= delta.x * m_rotSpeed;
    m_pitch -= delta.y * m_rotSpeed;

    // 짐벌락 방지 및 각도 제한
    if (m_yaw < 0.0f)   m_yaw += 360.0f;
    if (m_yaw > 360.0f) m_yaw -= 360.0f;

    if (m_pitch > 89.0f)  m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    GetTransform().SetRotation(glm::vec3(m_pitch, m_yaw, 0.0f));
}

