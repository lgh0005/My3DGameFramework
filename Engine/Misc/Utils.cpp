#include "EnginePch.h"
#include "Utils.h"

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

	return glm::vec3(kc, glm::max(kl, 0.0f), glm::max(kq * kq, 0.0f));
}

glm::mat4 Utils::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

glm::vec3 Utils::GetGLMVec(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat Utils::GetGLMQuat(const aiQuaternion& pOrientation)
{
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}