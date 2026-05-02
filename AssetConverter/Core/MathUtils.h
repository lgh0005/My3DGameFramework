#pragma once

namespace MGF3D
{
	class Math
	{
		MGF_DECLARE_UTILITIES(Math)

	public:

		// AABB 좌표를 얻어오는 유틸 함수
		static vec3 Min(const vec3& a, const vec3& b);
		static vec3 Max(const vec3& a, const vec3& b);

		// assimp to glm 유틸 함수
		static mat4 ConvertToGLMMat4(const aiMatrix4x4& aiMat);
		static vec3 ConvertToGLMVec(const aiVector3D& vec);
		static quat ConvertToGLMQuat(const aiQuaternion& pOrientation);

		// hdr 이미지 메모리를 절반으로 줄이는 유틸 함수
		static uint16 FloatToHalf(float f);
	};
}