#include "EnginePch.h"
#include "Camera.h"
#include "Components/Transform.h"

CameraUPtr Camera::Create()
{
    auto camera = CameraUPtr(new Camera());
    return std::move(camera);
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
}

void Camera::SetAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
    m_projectionMatrix = glm::perspective
    (
        glm::radians(m_fovDegrees),
        m_aspectRatio,
        m_nearPlane,
        m_farPlane
    );
}

glm::mat4 Camera::GetViewMatrix() const
{
    const Transform& transform = GetTransform();
    glm::vec3 position = transform.GetWorldPosition();
    glm::vec3 forward = transform.GetForwardVector();
    glm::vec3 up = transform.GetUpVector();
    return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
    return m_projectionMatrix * GetViewMatrix();
}

void Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
{
    Transform& transform = GetTransform();
    glm::vec3 position = transform.GetWorldPosition();
    glm::vec3 direction = glm::normalize(target - position);
    glm::quat rotation = glm::quatLookAt(direction, up);
    transform.SetRotation(rotation);
}
