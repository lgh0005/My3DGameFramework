#pragma once
#include "Resources/Resource.h"

#pragma region BONE_STRUCTS
struct BoneInfo
{
	uint32 id;
	glm::mat4 offset;
};

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};
#pragma endregion

// TODO : 이후 Legacy 클래스가 될 수 있음
// Skeleton의 컨테이너 역할 클래스로 역할이 대폭 축소 예정
CLASS_PTR(Bone)
class Bone : public Resource
{
public:
    static const ResourceType s_ResourceType = ResourceType::Bone;
    virtual ResourceType GetResourceType() const override { return ResourceType::Bone; }
	static BoneUPtr Create(const std::string& name, int id, const aiNodeAnim* channel);
    static BoneUPtr Create(uint32 id, std::vector<KeyPosition>&& positions,
                                      std::vector<KeyRotation>&& rotations,
                                      std::vector<KeyScale>&& scales);

public:
    void Update(float animationTime);
    uint32 GetPositionIndex(float animationTime);
    uint32 GetRotationIndex(float animationTime);
    uint32 GetScaleIndex(float animationTime);

    uint32 GetBoneID() { return m_id; }
    glm::mat4 GetLocalTransform() { return m_localTransform; }
    std::string GetBoneName() const { return m_name; }

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);

private:
	Bone() = default;
    bool Init(const std::string& name, int id, const aiNodeAnim* channel);
    void Init(uint32 id, std::vector<KeyPosition>&& positions,
                         std::vector<KeyRotation>&& rotations,
                         std::vector<KeyScale>&& scales);

    std::vector<KeyPosition> m_positions;
    std::vector<KeyRotation> m_rotations;
    std::vector<KeyScale>    m_scales;

    uint32 m_numPositions;
    uint32 m_numRotations;
    uint32 m_numScalings;

    glm::mat4   m_localTransform { glm::mat4(1.0f)};
    std::string m_name;
    uint32      m_id;
};