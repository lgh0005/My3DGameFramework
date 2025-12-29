#include "EnginePch.h"
#include "Pose.h"
#include <glm/gtx/matrix_decompose.hpp>

Pose::Pose() = default;
Pose::Pose(const glm::vec3& p, const glm::quat& r, const glm::vec3& s)
	: position(p), rotation(r), scale(s) {}
Pose::~Pose() = default;

glm::mat4 Pose::ToMat4() const
{
	glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 R = glm::toMat4(rotation);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
	return T * R * S;
}

Pose Pose::Interpolate(const Pose& a, const Pose& b, float factor)
{
	Pose result;
	result.position = glm::mix(a.position, b.position, factor);
	result.rotation = glm::normalize(glm::slerp(a.rotation, b.rotation, factor));
	result.scale = glm::mix(a.scale, b.scale, factor);
	return result;
}

Pose Pose::FromMat4(const glm::mat4& mat)
{
	Pose pose;
	glm::vec3 skew; glm::vec4 perspective;
	glm::decompose(mat, pose.scale, pose.rotation, pose.position, skew, perspective);
	return pose;
}

Pose Pose::Identity() { return Pose(); }

