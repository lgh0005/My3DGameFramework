#include "EnginePch.h"
#include "AnimClip.h"

DECLARE_DEFAULTS_IMPL(AnimClip)

AnimClip::AnimClip(AnimClip&& other) noexcept
	: localMatrices(std::move(other.localMatrices))
	, frameCount(other.frameCount)
	, boneCount(other.boneCount)
	, frameRate(other.frameRate)
{
	other.frameCount = 0;
	other.boneCount = 0;
}

AnimClip& AnimClip::operator=(AnimClip&& other) noexcept
{
	if (this != &other)
	{
		localMatrices = std::move(other.localMatrices);
		frameCount = other.frameCount;
		boneCount = other.boneCount;
		frameRate = other.frameRate;

		other.frameCount = 0;
		other.boneCount = 0;
	}
	return *this;
}

bool AnimClip::IsValid() const
{
	return !localMatrices.empty() && frameCount > 0;
}

void AnimClip::Clear()
{
	localMatrices.clear();
	frameCount = 0;
	boneCount = 0;
	frameRate = 0.0f;
}