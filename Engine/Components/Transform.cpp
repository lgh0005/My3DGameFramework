#include "Transform.h"
#include "EnginePch.h"

TransformUPtr Transform::Create()
{
	auto transform = TransformUPtr(new Transform());
	return transform;
}

/*====================//
//  rotation methods  //
//====================*/
void Transform::SetRotation(const glm::vec3& eulerAnglesDegrees)
{
	glm::vec3 eulerRadians = glm::radians(eulerAnglesDegrees);
	m_rotation = glm::quat(eulerRadians);
}

void Transform::Rotate(const glm::vec3& eulerAnglesDegrees)
{
	glm::vec3 eulerRadians = glm::radians(eulerAnglesDegrees);
	glm::quat deltaRotation = glm::quat(eulerRadians);
	m_rotation = m_rotation * deltaRotation;
	m_rotation = glm::normalize(m_rotation);
}

glm::vec3 Transform::GetRotationEuler() const
{
	glm::vec3 eulerRadians = glm::eulerAngles(m_rotation);
	return glm::degrees(eulerRadians);
}

void Transform::SetRotation(const glm::vec3& axis, float angleRadians)
{
	m_rotation = glm::angleAxis(angleRadians, glm::normalize(axis));
}

/*============================//
//  direction vector methods  //
//============================*/
glm::vec3 Transform::GetForwardVector() const
{
	return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Transform::GetUpVector() const
{
	return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Transform::GetRightVector() const
{
	return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

/*==========================//
//  calculate model matrix  //
//==========================*/
glm::mat4 Transform::GetModelMatrix() const
{
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), m_position);
	glm::mat4 rotMat = glm::toMat4(m_rotation);
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), m_scale);
	return transMat * rotMat * scaleMat;
}
