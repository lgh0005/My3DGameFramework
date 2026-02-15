#include "pch.h"
#include "Utils.h"

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

/*================================================//
//   hdr 이미지 메모리를 절반으로 줄이는 유틸 함수   //
//================================================*/
uint16 Utils::FloatToHalf(float f)
{
	uint32 i = *(uint32*)&f;
	uint32 s = (i >> 16) & 0x00008000;
	uint32 e = ((i >> 23) & 0x000000ff) - (127 - 15);
	uint32 m = i & 0x007fffff;

	if (e <= 0) 
	{
		if (e < -10) return (uint16)s;
		m = (m | 0x00800000) >> (1 - e);
		return (uint16)(s | (m >> 13));
	}
	else if (e >= 0x1f) 
	{
		return (uint16)(s | 0x7c00 | (m ? (m >> 13) | 1 : 0));
	}

	return (uint16)(s | (e << 10) | (m >> 13));
}
