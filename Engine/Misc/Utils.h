#pragma once
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Core/Color.h>

class Utils
{
	DECLARE_STATIC_CLASS(Utils)

public:

	// 조명 연산 전용 유틸 함수
	static glm::vec3 GetAttenuationCoeff(float distance);

	// 셰이더 파일 로드 유틸 함수
	static std::optional<std::string> LoadTextFile(const std::string& filename);

	// assimp to glm 유틸 함수
	static glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& aiMat);
	static glm::vec3 ConvertToGLMVec(const aiVector3D& vec);
	static glm::quat ConvertToGLMQuat(const aiQuaternion& pOrientation);

	// Jolt to glm 유틸 함수
	static glm::vec3 ToGlmVec3(const JPH::Vec3& v);
	static glm::quat ToGlmQuat(const JPH::Quat& q);
	static glm::vec4 ToGlmColor(const JPH::ColorArg color);

	// glm to Jolt 유틸 함수
	static JPH::Vec3 ToJoltVec3(const glm::vec3& v);
	static JPH::Quat ToJoltQuat(const glm::quat& q);

	// SSAO 연산을 위한 Lerp 함수
	static float Lerp(float a, float b, float f);

	// AABB 좌표를 얻어오는 유틸 함수
	static glm::vec3 Min(const glm::vec3& a, const glm::vec3& b);
	static glm::vec3 Max(const glm::vec3& a, const glm::vec3& b);


/*==================================//
//   Common Constants (공용 상수)    //
//==================================*/
public:
	inline static const glm::mat4 IdentityMat4 = glm::mat4(1.0f);
};