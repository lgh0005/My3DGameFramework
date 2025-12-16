#pragma once
#include "Resources/Bone.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
CLASS_PTR(Bone)
#pragma endregion

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int32 childrenCount;
    std::vector<AssimpNodeData> children;
};

CLASS_PTR(Animation)
class Animation
{
public:
	static AnimationUPtr Load(const std::string& filePath, Model* model);

    Bone* FindBone(const std::string& name);
    float GetTicksPerSecond() { return m_ticksPerSecond; }
    float GetDuration() { return m_duration; }
    const AssimpNodeData& GetRootNode() { return m_rootNode; }
    const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap() { return m_boneInfoMap; }

private:
	Animation() = default;

    // 로딩 함수 분리
    bool LoadByAssimp(const std::string& filePath, Model* model);
    bool LoadByBinary(const std::string& filePath);

    void ReadMissingBones(const aiAnimation* animation, Model& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_duration;
    int32 m_ticksPerSecond;
    std::vector<BoneUPtr> m_bones;
    AssimpNodeData m_rootNode;
    std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
};