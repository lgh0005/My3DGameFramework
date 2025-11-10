#pragma once

#pragma region FORWARD_DECLARATION
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType : int32;
#pragma endregion

#pragma region CONVERT_STRUCTS

struct TempKeyPosition 
{ 
	glm::vec3 position; 
	float timeStamp; 
};

struct TempKeyRotation 
{ 
	glm::quat orientation; 
	float timeStamp; 
};

struct TempKeyScale 
{ 
	glm::vec3 scale; 
	float timeStamp; 
};

struct TempBoneChannel
{
	uint32 boneID;
	std::string boneName;
	std::vector<TempKeyPosition> positions;
	std::vector<TempKeyRotation> rotations;
	std::vector<TempKeyScale>    scales;
};

#pragma endregion

class AnimConverter
{
public:
	static bool Convert(const std::string& animPath,
						const std::string& modelPath,
						const std::string& outputPath);

private:
	AnimConverter() = default;
	bool RunConversion(const std::string& animPath,
					   const std::string& modelPath,
					   const std::string& outputPath);

	bool LoadSkeletonFromMyModel(const std::string& modelPath);
	void ProcessAnimation(const aiAnimation* anim);
	TempBoneChannel ProcessBoneChannel(const aiNodeAnim* channel);
	bool WriteCustomAnimFile(const std::string& outputPath);

private:
	std::unordered_map<std::string, uint32_t> m_boneNameToIdMap;
	std::string m_animName;
	float m_duration = 0.0f;
	float m_ticksPerSecond = 0.0f;
	std::vector<TempBoneChannel> m_boneChannels;
};