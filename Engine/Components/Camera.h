#pragma once
#include "Component.h"

CLASS_PTR(Camera)
class Camera : public Component
{
public:
	virtual ~Camera();
	static CameraUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Camera;
	virtual ComponentType GetComponentType() const override { return ComponentType::Camera; }

	void SetProjection(float fovDegrees, float aspectRatio, 
					   float nearPlane, float farPlane);
	void SetAspectRatio(float aspectRatio);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix() const;

	void LookAt(const glm::vec3& target, 
				const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

private:
	Camera();

	float m_fovDegrees	{ 45.0f };
	float m_aspectRatio	{ (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT};
	float m_nearPlane	{ 0.1f };
	float m_farPlane	{ 100.0f };

	glm::mat4 m_projectionMatrix	{ glm::mat4(1.0f) };
};
