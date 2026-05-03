#include "FrameworkPch.h"
#include "AnimChannel.h"

namespace MGF3D
{
	AnimChannel::AnimChannel() = default;
	AnimChannel::~AnimChannel() = default;

	AnimChannelUPtr AnimChannel::Create
	(
		const String& name,
		Vector<RawKeyPosition>&& positions,
		Vector<RawKeyRotation>&& rotations,
		Vector<RawKeyScale>&& scales
	)
	{
		auto animChannel = AnimChannelUPtr(new AnimChannel());
		animChannel->Init
		(
			name,
			std::move(positions), std::move(rotations), std::move(scales)
		);
		return animChannel;
	}

	void AnimChannel::Init
	(
		const String& name,
		Vector<RawKeyPosition>&& positions,
		Vector<RawKeyRotation>&& rotations,
		Vector<RawKeyScale>&& scales
	)
	{
		m_name = name;
		m_nameHash = StringHash(name);

		m_positions = std::move(positions);
		m_rotations = std::move(rotations);
		m_scales = std::move(scales);

		m_numPositions = static_cast<uint32>(m_positions.size());
		m_numRotations = static_cast<uint32>(m_rotations.size());
		m_numScalings = static_cast<uint32>(m_scales.size());
	}

	Pose AnimChannel::GetPose(float time) const
	{
		vec3 translation = InterpolatePosition(time);
		quat rotation = InterpolateRotation(time);
		vec3 scale = InterpolateScaling(time);
		return Pose(translation, rotation, scale);
	}

	/*==============================================//
	//   animation channel default getter methods   //
	//==============================================*/
	StringHash AnimChannel::GetNameHash() const { return m_nameHash; }

	const String& AnimChannel::GetBoneName() const { return m_name; }

	mat4 AnimChannel::GetLocalTransform(float animationTime) const
	{
		return GetPose(animationTime).ToMat4();
	}

	uint32 AnimChannel::GetPositionIndex(float animationTime) const
	{
		for (uint32 index = 0; index < m_numPositions - 1; ++index)
		{
			if (animationTime < m_positions[index + 1].time) return index;
		}
		return m_numPositions - 2;
	}

	uint32 AnimChannel::GetRotationIndex(float animationTime) const
	{
		for (uint32 index = 0; index < m_numRotations - 1; ++index)
		{
			if (animationTime < m_rotations[index + 1].time) return index;
		}
		return m_numRotations - 2;
	}

	uint32 AnimChannel::GetScaleIndex(float animationTime) const
	{
		for (uint32 index = 0; index < m_numScalings - 1; ++index)
		{
			if (animationTime < m_scales[index + 1].time) return index;
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

	vec3 AnimChannel::InterpolatePosition(float animationTime) const
	{
		if (m_numPositions == 1) return m_positions[0].position;

		uint32 p0Index = GetPositionIndex(animationTime);
		uint32 p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_positions[p0Index].time, m_positions[p1Index].time, animationTime);

		return glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);
	}

	quat AnimChannel::InterpolateRotation(float animationTime) const
	{
		if (m_numRotations == 1) return Math::Normalize(m_rotations[0].rotation);

		uint32 p0Index = GetRotationIndex(animationTime);
		uint32 p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_rotations[p0Index].time, m_rotations[p1Index].time, animationTime);

		quat finalRotation = Math::Slerp(m_rotations[p0Index].rotation, m_rotations[p1Index].rotation, scaleFactor);
		return Math::Normalize(finalRotation);
	}

	vec3 AnimChannel::InterpolateScaling(float animationTime) const
	{
		if (m_numScalings == 1) return m_scales[0].scale;

		uint32 p0Index = GetScaleIndex(animationTime);
		uint32 p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_scales[p0Index].time, m_scales[p1Index].time, animationTime);

		return glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);
	}
}
