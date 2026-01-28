#include "EnginePch.h"
#include "Camera.h"
#include "Components/Transform.h"
#include "Graphics/Raycasting/Ray.h"
#include "Graphics/Buffers/Velocitybuffer.h"

Camera::Camera()
{
    m_lastRawPosition = glm::vec3(0.0f);
}
Camera::~Camera() = default;

CameraUPtr Camera::Create()
{
    auto cam = CameraUPtr(new Camera());
    if (!cam->Init()) return nullptr;
    return std::move(cam);
}

bool Camera::Init()
{
    m_posVelocityBuffer = Velocitybuffer::Create();
    m_rotVelocityBuffer = Velocitybuffer::Create();
    if (!m_posVelocityBuffer) return false;
    return true;
}

void Camera::OnStart()
{
    m_lastRawForward = GetTransform().GetForwardVector();
}

void Camera::OnUpdate()
{
    UpdateVelocity(TIME.GetDeltaTime());
}

glm::mat4 Camera::GetVirtualPrevViewProjectionMatrix(float targetShutterSpeed) const
{
    // 1. 위치 역산 (기존)
    glm::vec3 smoothDisplacement = m_posVelocityBuffer->GetSmoothVelocity(targetShutterSpeed);
    glm::vec3 currentPos = GetTransform().GetWorldPosition();
    glm::vec3 virtualPrevPos = currentPos - smoothDisplacement * 2.0f;

    // 2. 방향(Forward) 역산 (회전은 그대로 둠 - 보통 회전은 1.0으로도 충분히 강함)
    glm::vec3 smoothDeltaForward = m_rotVelocityBuffer->GetSmoothVelocity(targetShutterSpeed);
    glm::vec3 currentForward = GetTransform().GetForwardVector();
    glm::vec3 virtualPrevForward = glm::normalize(currentForward - smoothDeltaForward);

    // 3. 행렬 재구성
    glm::vec3 up = GetTransform().GetUpVector();
    glm::mat4 virtualPrevView = glm::lookAt(virtualPrevPos, virtualPrevPos + virtualPrevForward, up);

    return m_projectionMatrix * virtualPrevView;
}

void Camera::UpdateVelocity(float dt)
{
    glm::vec3 currentPos = GetTransform().GetWorldPosition();
    glm::vec3 currentForward = GetTransform().GetForwardVector();

    if (dt > 1e-6f)
    {
        // 1. 위치 기록 (기존)
        glm::vec3 displacement = currentPos - m_lastRawPosition;
        m_posVelocityBuffer->RecordMovement(displacement, dt);

        // 2. ★ 회전(방향) 기록
        // 방향 벡터의 차이를 속도처럼 기록합니다.
        glm::vec3 deltaForward = currentForward - m_lastRawForward;
        m_rotVelocityBuffer->RecordMovement(deltaForward, dt);
    }

    m_lastRawPosition = currentPos;
    m_lastRawForward = currentForward;
}

void Camera::SetProjection(float fovDegrees, float aspectRatio, 
                           float nearPlane, float farPlane)
{
    m_fovDegrees = fovDegrees;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    m_projectionMatrix = glm::perspective
    (
        glm::radians(m_fovDegrees),
        m_aspectRatio,
        m_nearPlane,
        m_farPlane
    );

    m_invProjectionMatrix = glm::inverse(m_projectionMatrix);
    m_isProjectionDirty = true;
}

void Camera::SetViewportSize(float width, float height)
{
    if (width <= 0 || height <= 0) return;
    m_aspectRatio = width / height;

    // 1. 3D Perspective 재계산 (Eager Evaluation)
    m_projectionMatrix = glm::perspective
    (
        glm::radians(m_fovDegrees), 
        m_aspectRatio, 
        m_nearPlane, m_farPlane
    );
    m_invProjectionMatrix = glm::inverse(m_projectionMatrix);

    // 2. 2D UI Ortho 재계산 (Eager Evaluation)
    m_orthoProjectionMatrix = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);

    // 3. VP 매트릭스 갱신 요청
    m_isProjectionDirty = true;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return GetTransform().GetWorldInverseMatrix();
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
    // 1. 현재의 최신 View Matrix를 가져옴 (Transform이 캐싱된 값 리턴)
    const glm::mat4& currentView = GetViewMatrix();

    // 2. 변경 감지: 투영이 바뀌었거나 OR 뷰(위치/회전)가 이전 프레임과 다르면 갱신
    if (m_isProjectionDirty || currentView != m_lastViewMatrix)
    {
        m_viewProjectionMatrix = m_projectionMatrix * currentView;

        // 상태 동기화
        m_lastViewMatrix = currentView;
        m_isProjectionDirty = false;
    }

    return m_viewProjectionMatrix;
}

void Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
{
    Transform& transform = GetTransform();
    glm::vec3 position = transform.GetWorldPosition();
    glm::vec3 direction = glm::normalize(target - position);
    glm::quat rotation = glm::quatLookAt(direction, up);
    transform.SetRotation(rotation);
}

Ray Camera::ScreenPointToRay(const glm::vec2& screenPos, const glm::vec2& screenSize)
{
    if (screenSize.x <= 0 || screenSize.y <= 0) return Ray();

    float x = (2.0f * screenPos.x) / screenSize.x - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / screenSize.y;

    // 1. Projection 역행렬 사용 (미리 계산됨)
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = m_invProjectionMatrix * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // 2. View 역행렬 (= World Matrix) 사용
    const glm::mat4& cameraWorldMatrix = GetTransform().GetWorldMatrix();
    glm::vec3 rayDirWorld = glm::vec3(cameraWorldMatrix * rayEye);

    // 3. Ray 반환
    return Ray(GetTransform().GetWorldPosition(), glm::normalize(rayDirWorld));
}
