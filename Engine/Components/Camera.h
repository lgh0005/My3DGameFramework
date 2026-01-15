#pragma once
#include "Object/Component.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Ray)
#pragma endregion

CLASS_PTR(Camera)
class Camera : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::Camera)

public:
	virtual ~Camera();
	static CameraUPtr Create();

	void SetProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
	void SetViewportSize(float width, float height);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix() const;
	const glm::mat4& GetOrthoProjectionMatrix() const { return m_orthoProjectionMatrix; }

	void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
	Ray ScreenPointToRay(const glm::vec2& screenPos, const glm::vec2& screenSize);

private:
	Camera();

	float m_fovDegrees	{ 45.0f };
	float m_aspectRatio	{ (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT};
	float m_nearPlane	{ 0.1f };
	float m_farPlane	{ 100.0f };

	glm::mat4 m_projectionMatrix		{ 1.0f };
	glm::mat4 m_invProjectionMatrix		{ 1.0f };
	glm::mat4 m_orthoProjectionMatrix	{ 1.0f };

	// ViewProjection 캐싱을 위한 변수
	mutable glm::mat4 m_viewProjectionMatrix	{ 1.0f };
	mutable glm::mat4 m_lastViewMatrix			{ 1.0f }; // 변경 감지용
	mutable bool m_isProjectionDirty			{ true }; // 투영 변경 감지용
};
