#pragma once

class Utils
{
public:

	// AABB 좌표를 얻어오는 유틸 함수
	static glm::vec3 Min(const glm::vec3& a, const glm::vec3& b);
	static glm::vec3 Max(const glm::vec3& a, const glm::vec3& b);

	// assimp to glm 유틸 함수
	static glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& aiMat);
	static glm::vec3 ConvertToGLMVec(const aiVector3D& vec);
	static glm::quat ConvertToGLMQuat(const aiQuaternion& pOrientation);

	// hdr 이미지 메모리를 절반으로 줄이는 유틸 함수
	static uint16 FloatToHalf(float f);
};