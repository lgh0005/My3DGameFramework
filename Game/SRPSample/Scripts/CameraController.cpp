#include "pch.h"
#include "CameraController.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/MeshOutline.h"
#include "Object/GameObject.h"
#include "PlayerController.h"
#include "Graphics/Ray.h"
#include "Graphics/RaycastHit.h"

DECLARE_DEFAULTS_IMPL(CameraController)

CameraControllerUPtr CameraController::Create()
{
    auto script = CameraControllerUPtr(new CameraController());
    return std::move(script);
}

void CameraController::Start()
{
    m_camera = GetOwner()->GetComponent<Camera>();

    m_outlines.reserve(256);

    glm::vec3 rotation = GetTransform().GetRotationEuler();
    m_pitch = rotation.x;
    m_yaw = rotation.y;

    INPUT_MGR.MapAction("MoveForward", GLFW_KEY_W);
    INPUT_MGR.MapAction("MoveBackward", GLFW_KEY_S);
    INPUT_MGR.MapAction("MoveLeft", GLFW_KEY_A);
    INPUT_MGR.MapAction("MoveRight", GLFW_KEY_D);
    INPUT_MGR.MapAction("MoveUp", GLFW_KEY_E);
    INPUT_MGR.MapAction("MoveDown", GLFW_KEY_Q);

    INPUT_MGR.MapMouseAction("ControlCamera", GLFW_MOUSE_BUTTON_RIGHT);
    INPUT_MGR.MapMouseAction("PickObject", GLFW_MOUSE_BUTTON_LEFT);
}

void CameraController::Update()
{
    auto deltaTime = TIME.GetDeltaTime();
    HandleMovement(deltaTime);
    HandleRotation(deltaTime);
    HandlePicking();
}

void CameraController::HandleMovement(float dt)
{
    auto& transform = GetTransform();
    float speed = m_moveSpeed * dt;

    if (INPUT_MGR.GetButton("MoveForward"))
        transform.Translate(transform.GetForwardVector() * speed);
    if (INPUT_MGR.GetButton("MoveBackward")) 
        transform.Translate(transform.GetForwardVector() * -speed);

    if (INPUT_MGR.GetButton("MoveRight"))    
        transform.Translate(transform.GetRightVector() * speed);
    if (INPUT_MGR.GetButton("MoveLeft"))     
        transform.Translate(transform.GetRightVector() * -speed);

    if (INPUT_MGR.GetButton("MoveUp"))       
        transform.Translate(transform.GetUpVector() * speed);
    if (INPUT_MGR.GetButton("MoveDown"))     
        transform.Translate(transform.GetUpVector() * -speed);
}

void CameraController::HandleRotation(float dt)
{
    if (INPUT_MGR.GetButtonDown("ControlCamera"))
    {
        m_controlEnabled = true;
        m_prevMousePos = INPUT_MGR.GetMousePos(); // 클릭 순간 튀는 현상 방지
    }
    else if (INPUT_MGR.GetButtonUp("ControlCamera"))
    {
        m_controlEnabled = false;
    }
    if (!m_controlEnabled) return;

    // 마우스 델타 계산
    glm::vec2 currentPos = INPUT_MGR.GetMousePos();
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

void CameraController::HandlePicking()
{
    if (!m_camera) return;

    // 마우스 좌클릭 시
    if (INPUT_MGR.GetButtonDown("PickObject"))
    {
        // 1. Ray 생성
        glm::vec2 mousePos = INPUT_MGR.GetMousePos();
        int32 windowWidth = WINDOW.GetWindowWidth();
        int32 windowHeight = WINDOW.GetWindowHeight();
        Ray ray = m_camera->ScreenPointToRay
        (
            glm::vec2(mousePos.x, mousePos.y), 
            glm::vec2(windowWidth, windowHeight)
        );

        // 2. Raycast 수행
        RaycastHit hit;
        float maxDist = 1000.0f;

        if (PHYSICS.Raycast(ray, maxDist, hit))
        {
            GameObject* hitLeaf = hit.GetGameObject();
            GameObject* hitRoot = hitLeaf->GetRoot();

            if (m_lastSelected == hitRoot)
            {
                m_outlines.clear();
                hitRoot->GetComponentsInChildren<MeshOutline>(m_outlines);
                for (auto* outline : m_outlines) outline->SetEnable(false);

                m_lastSelected = nullptr; // 선택 해제
                LOG_INFO("[Picking] Toggle Off: {}", hitRoot->GetName());
            }
            else
            {
                if (m_lastSelected)
                {
                    m_outlines.clear();
                    m_lastSelected->GetComponentsInChildren<MeshOutline>(m_outlines);
                    for (auto* outline : m_outlines) outline->SetEnable(false);
                }

                m_outlines.clear();
                hitRoot->GetComponentsInChildren<MeshOutline>(m_outlines);
                for (auto* outline : m_outlines) outline->SetEnable(true);

                m_lastSelected = hitRoot;
                LOG_INFO("[Picking] Selected: {}", hitRoot->GetName());
            }
        }

        // 3. 허공 클릭 (Miss)
        else
        {
            // 선택 해제: 이전에 선택된 놈이 있다면 끄기
            if (m_lastSelected)
            {
                m_outlines.clear();
                m_lastSelected->GetComponentsInChildren<MeshOutline>(m_outlines);
                for (auto* outline : m_outlines) outline->SetEnable(false);

                m_lastSelected = nullptr;
                LOG_INFO("[Picking] Deselected.");
            }
        }
    }
}

