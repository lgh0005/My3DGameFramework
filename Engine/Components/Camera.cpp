#include "EnginePch.h"
#include "Camera.h"
#include "Transform.h"

CameraUPtr Camera::Create()
{
    auto camera = CameraUPtr(new Camera());
    if (!camera->Init()) return nullptr;
    return std::move(camera);
}

bool Camera::Init()
{
    m_transform = Transform::Create();
    if (!m_transform) return false;

    SetProjection(m_fovDegrees, m_aspectRatio, m_nearPlane, m_farPlane);
    return true;
}

void Camera::SetProjection(float fovDegrees, float aspectRatio, 
                           float nearPlane, float farPlane)
{
    m_fovDegrees = fovDegrees;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    m_projectionMatrix = glm::perspective(glm::radians(m_fovDegrees),
        m_aspectRatio,
        m_nearPlane,
        m_farPlane);
}

glm::mat4 Camera::GetViewMatrix() const
{
    glm::vec3 position = m_transform->GetPosition();
    glm::vec3 forward = m_transform->GetForwardVector();
    glm::vec3 up = m_transform->GetUpVector();
    return glm::lookAt(position, position + forward, up);
}