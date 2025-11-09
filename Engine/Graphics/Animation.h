#pragma once

// TODO : include 문제 해결 필요

// [추가] 이 헤더가 스스로 완전해지도록 모든 의존성을 포함합니다.
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

// [수정] BoneInfo의 *정의*가 필요하므로 Bone.h를 include합니다.
#include "Graphics/Bone.h"

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
	static AnimationUPtr Create(const std::string& animationPath, Model* model);

    Bone* FindBone(const std::string& name);
    float GetTicksPerSecond() { return m_ticksPerSecond; }
    float GetDuration() { return m_duration; }
    const AssimpNodeData& GetRootNode() { return m_rootNode; }
    const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap() { return m_boneInfoMap; }

private:
    void ReadMissingBones(const aiAnimation* animation, Model& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

private:
	Animation() = default;
    bool Init(const std::string& animationPath, Model* model);

    float m_duration;
    int32 m_ticksPerSecond;
    std::vector<BoneUPtr> m_bones;
    AssimpNodeData m_rootNode;
    std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
};