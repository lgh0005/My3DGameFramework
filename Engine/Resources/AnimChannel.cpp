#include "EnginePch.h"
#include "Resources/AnimChannel.h"
#include "Resources/Pose.h"

AnimChannel::AnimChannel() = default;
AnimChannel::~AnimChannel() = default;

AnimChannelUPtr AnimChannel::Create(const std::string& name, int32 id, const aiNodeAnim* channel)
{
    auto animChannel = AnimChannelUPtr(new AnimChannel());
    if (!animChannel->Init(name, id, channel)) return nullptr;
    return std::move(animChannel);
}

AnimChannelUPtr AnimChannel::Create
(
    const std::string& name,
    uint32 id, 
    std::vector<AssetFmt::RawKeyPosition>&& positions,
    std::vector<AssetFmt::RawKeyRotation>&& rotations,
    std::vector<AssetFmt::RawKeyScale>&& scales
)
{
    auto animChannel = AnimChannelUPtr(new AnimChannel());
    animChannel->Init
    (
        name, id, 
        std::move(positions), std::move(rotations), std::move(scales)
    );
    return std::move(animChannel);
}

Pose AnimChannel::GetPose(float time) const
{
    glm::vec3 translation = InterpolatePosition(time);
    glm::quat rotation = InterpolateRotation(time);
    glm::vec3 scale = InterpolateScaling(time);
    return Pose(translation, rotation, scale);
}

bool AnimChannel::Init(const std::string& name, int id, const aiNodeAnim* channel)
{
    if (!channel) return false;

	m_name = name;
	m_id = id;

	m_numPositions = channel->mNumPositionKeys;
    for (int positionIndex = 0; positionIndex < m_numPositions; ++positionIndex)
    {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;

        AssetFmt::RawKeyPosition data;
        data.vec3 = Utils::ConvertToGLMVec(aiPosition);
        data.time = timeStamp;
        m_positions.push_back(data);
    }

    m_numRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_numRotations; ++rotationIndex)
    {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;

        AssetFmt::RawKeyRotation data;
        data.quat = Utils::ConvertToGLMQuat(aiOrientation);
        data.time = timeStamp;
        m_rotations.push_back(data);
    }

    m_numScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_numScalings; ++keyIndex)
    {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;

        AssetFmt::RawKeyScale data;
        data.vec3 = Utils::ConvertToGLMVec(scale);
        data.time = timeStamp;
        m_scales.push_back(data);
    }

	return true;
}

void AnimChannel::Init
(
    const std::string& name,
    uint32 id, 
    std::vector<AssetFmt::RawKeyPosition>&& positions,
    std::vector<AssetFmt::RawKeyRotation>&& rotations,
    std::vector<AssetFmt::RawKeyScale>&& scales
)
{
    m_name = name;
    m_id = id;

    m_positions = std::move(positions);
    m_rotations = std::move(rotations);
    m_scales = std::move(scales);

    m_numPositions = (uint32)m_positions.size();
    m_numRotations = (uint32)m_rotations.size();
    m_numScalings = (uint32)m_scales.size();
}

/*==============================================//
//   animation channel default getter methods   //
//==============================================*/
uint32 AnimChannel::GetBoneID() const { return m_id; }

std::string AnimChannel::GetBoneName() const { return m_name; }

glm::mat4 AnimChannel::GetLocalTransform(float animationTime) const
{
    return GetPose(animationTime).ToMat4();
}

uint32 AnimChannel::GetPositionIndex(float animationTime) const 
{
    for (int index = 0; index < m_numPositions - 1; ++index)
    {
        if (animationTime < m_positions[index + 1].time)
            return index;
    }

    return m_numPositions - 2;
}

uint32 AnimChannel::GetRotationIndex(float animationTime) const 
{
    for (int index = 0; index < m_numRotations - 1; ++index)
    {
        if (animationTime < m_rotations[index + 1].time)
            return index;
    }

    return m_numRotations - 2;
}

uint32 AnimChannel::GetScaleIndex(float animationTime) const
{
    for (int index = 0; index < m_numScalings - 1; ++index)
    {
        if (animationTime < m_scales[index + 1].time)
            return index;
    }

    return m_numScalings - 2;
}

/*===========================//
//   interpolation methods   //
//===========================*/
float AnimChannel::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const 
{
    float framesDiff = nextTimeStamp - lastTimeStamp;
    if (framesDiff <= 0.0001f) return 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    return midWayLength / framesDiff;
}

glm::vec3 AnimChannel::InterpolatePosition(float animationTime) const
{
    if (m_numPositions == 1) return m_positions[0].vec3;

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_positions[p0Index].time,
        m_positions[p1Index].time, animationTime);

    return glm::mix(m_positions[p0Index].vec3, m_positions[p1Index].vec3, scaleFactor);
}

glm::quat AnimChannel::InterpolateRotation(float animationTime) const
{
    if (m_numRotations == 1) return glm::normalize(m_rotations[0].quat);

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_rotations[p0Index].time,
        m_rotations[p1Index].time, animationTime);

    glm::quat finalRotation = glm::slerp(m_rotations[p0Index].quat,
        m_rotations[p1Index].quat, scaleFactor);

    return glm::normalize(finalRotation);
}

glm::vec3 AnimChannel::InterpolateScaling(float animationTime) const
{
    if (m_numScalings == 1) return m_scales[0].vec3;

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_scales[p0Index].time,
        m_scales[p1Index].time, animationTime);

    return glm::mix(m_scales[p0Index].vec3, m_scales[p1Index].vec3, scaleFactor);
}

