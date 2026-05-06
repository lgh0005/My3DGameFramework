#include "FrameworkPch.h"
#include "AnimClip.h"

namespace MGF3D
{
	AnimClip::AnimClip() = default;
	AnimClip::AnimClip
	(
		uint32 frameCount,
		uint32 boneCount,
		float frameRate,
		Vector<mat4>&& matrices
	) : m_frameCount(frameCount)
		, m_boneCount(boneCount)
		, m_frameRate(frameRate)
		, m_localMatrices(std::move(matrices)) { }
	AnimClip::~AnimClip() = default;
	AnimClip::AnimClip(AnimClip&& other) noexcept = default;
	AnimClip& AnimClip::operator=(AnimClip&& other) noexcept = default;

	bool AnimClip::IsValid() const
	{
		return !m_localMatrices.empty() && m_frameCount > 0 && m_boneCount > 0;
	}

	void AnimClip::Clear()
	{
		m_localMatrices.clear();
		m_frameCount = 0;
		m_boneCount = 0;
		m_frameRate = 0.0f;
	}

	float AnimClip::GetDuration() const
	{
		if (m_frameRate <= 0.0f) return 0.0f;
		return static_cast<float>(m_frameCount) / m_frameRate;
	}

	const mat4& AnimClip::GetLocalMatrix(uint32 frame, uint32 boneIndex) const
	{
		if (frame >= m_frameCount) frame = m_frameCount - 1;
		if (boneIndex >= m_boneCount) boneIndex = 0;
		return m_localMatrices[frame * m_boneCount + boneIndex];
	}
}