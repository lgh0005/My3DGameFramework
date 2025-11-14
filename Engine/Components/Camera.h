#pragma once
#include "Components/Bases/Component.h"

CLASS_PTR(Camera)
class Camera : public Component
{
public:
	static CameraUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Camera;
	virtual ComponentType GetType() const override { return ComponentType::Camera; }

	void SetProjection(float fovDegrees, float aspectRatio, 
					   float nearPlane, float farPlane);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	void LookAt(const glm::vec3& target, 
				const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

private:
	Camera() = default;

	float m_fovDegrees	{ 45.0f };
	float m_aspectRatio	{ (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT};
	float m_nearPlane	{ 0.1f };
	float m_farPlane	{ 100.0f };

	glm::mat4 m_projectionMatrix	{ glm::mat4(1.0f) };
};
