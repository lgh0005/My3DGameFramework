#include "EnginePch.h"
#include "Utils.h"
#include <glm/gtx/matrix_decompose.hpp>

/*============================//
//   조명 연산 전용 유틸 함수   //
//============================*/
// 조명 감쇠 함수 : 거리를 넣으면 그에 따른 조명 감쇠 계수를 계산해준다.
// 경험적으로 얻은 계수에 대한 근사식
glm::vec3 Utils::GetAttenuationCoeff(float distance)
{
	const auto linear_coeff = glm::vec4
	(
		8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01
	);

	const auto quad_coeff = glm::vec4
	(
		-7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02
	);

	float kc = 1.0f;
	float d = 1.0f / distance;
	auto dvec = glm::vec4(1.0f, d, d * d, d * d * d);
	float kl = glm::dot(linear_coeff, dvec);
	float kq = glm::dot(quad_coeff, dvec);

	return glm::vec3(kc, glm::max<float>(kl, 0.0f), glm::max<float>(kq * kq, 0.0f));
}

/*==============================//
//   셰이더 파일 로드 유틸 함수   //
//==============================*/
std::optional<std::string> Utils::LoadTextFile(const std::string& filename)
{
	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		LOG_ERROR("failed to open file: {}", filename);
		return {};
	}
	std::stringstream text;
	text << fin.rdbuf();
	return text.str();
}

/*============================//
//   assimp to glm 유틸 함수   //
//============================*/
glm::mat4 Utils::ConvertToGLMMat4(const aiMatrix4x4& from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

glm::vec3 Utils::ConvertToGLMVec(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat Utils::ConvertToGLMQuat(const aiQuaternion& pOrientation)
{
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

/*==========================//
//   Jolt to glm 유틸 함수   //
//==========================*/
glm::vec3 Utils::ToGlmVec3(const JPH::Vec3& v)
{
	return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

glm::quat Utils::ToGlmQuat(const JPH::Quat& q)
{
	return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
}

glm::vec4 Utils::ToGlmColor(const JPH::ColorArg color)
{
	return glm::vec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

/*==========================//
//   glm to Jolt 유틸 함수   //
//==========================*/
JPH::Vec3 Utils::ToJoltVec3(const glm::vec3& v)
{
	return JPH::Vec3(v.x, v.y, v.z);
}

JPH::Quat Utils::ToJoltQuat(const glm::quat& q)
{
	return JPH::Quat(q.x, q.y, q.z, q.w);
}

/*===============================//
//   SSAO 연산을 위한 Lerp 함수   //
//===============================*/
float Utils::Lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

/*==================================//
//   AABB 좌표를 얻어오는 유틸 함수   //
//==================================*/
glm::vec3 Utils::Min(const glm::vec3& a, const glm::vec3& b)
{
	// 각 성분별로 최솟값을 취하여 새로운 vec3를 생성
	return glm::vec3
	(
		std::min<float>(a.x, b.x),
		std::min<float>(a.y, b.y),
		std::min<float>(a.z, b.z)
	);
}

glm::vec3 Utils::Max(const glm::vec3& a, const glm::vec3& b)
{
	// 각 성분별로 최댓값을 취하여 새로운 vec3를 생성
	return glm::vec3
	(
		glm::max<float>(a.x, b.x),
		std::max<float>(a.y, b.y),
		std::max<float>(a.z, b.z)
	);
}

