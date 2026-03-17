#include "GraphicsPch.h"
#include "Animation/Pose.h"

namespace MGF3D
{
	Pose::Pose() = default;
	Pose::Pose(const vec3& p, const quat& r, const vec3& s)
		: position(p), rotation(r), scale(s) { }
	Pose::~Pose() = default;

	mat4 Pose::ToMat4() const
	{
		mat4 T = glm::translate(mat4(1.0f), position);
		mat4 R = glm::toMat4(rotation);
		mat4 S = glm::scale(mat4(1.0f), scale);
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

	Pose Pose::FromMat4(const mat4& mat)
	{
		Pose pose;
		vec3 skew; glm::vec4 perspective;
		glm::decompose(mat, pose.scale, pose.rotation, pose.position, skew, perspective);
		return pose;
	}

	Pose Pose::Identity() { return Pose(); }
}


