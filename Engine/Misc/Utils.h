#pragma once
#include <random>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Utils
{
public:

	// 조명 연산 전용 유틸 함수
	static glm::vec3 GetAttenuationCoeff(float distance);

	// assimp 애니메이션 전용 유틸 함수
	static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& aiMat);
	static glm::vec3 GetGLMVec(const aiVector3D& vec);
	static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);

	// AABB 좌표를 얻어오는 유틸 함수
	static glm::vec3 Min(const glm::vec3& a, const glm::vec3& b);
	static glm::vec3 Max(const glm::vec3& a, const glm::vec3& b);

	// Scene 스크립팅 API
};