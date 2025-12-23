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

	// assimp to glm 유틸 함수
	static glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& aiMat);
	static glm::vec3 ConvertToGLMVec(const aiVector3D& vec);
	static glm::quat ConvertToGLMQuat(const aiQuaternion& pOrientation);

	// SSAO 연산을 위한 Lerp 함수
	static float Lerp(float a, float b, float f);

	// AABB 좌표를 얻어오는 유틸 함수
	static glm::vec3 Min(const glm::vec3& a, const glm::vec3& b);
	static glm::vec3 Max(const glm::vec3& a, const glm::vec3& b);

	// TEMP : 애니메이션 블렌딩을 위한 트랜스폼 보간 유틸 함수
	static glm::mat4 Interpolate(const glm::mat4& matA, const glm::mat4& matB, float blendFactor);
};