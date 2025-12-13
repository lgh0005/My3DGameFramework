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

	// ModelConverter의 파일 기록 유틸 함수
	template<typename T>
	static void WriteData(std::ofstream& file, const T& data)
	{
		file.write(reinterpret_cast<const char*>(&data), sizeof(T));
	}

	static void WriteString(std::ofstream& file, const std::string& str);
};