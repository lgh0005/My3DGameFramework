#pragma once
#include "Utils/AnimChannel.h"

class AnimationConverter
{
	DECLARE_SINGLE(AnimationConverter)

public:
	bool Convert
	(
		const std::string& animPath,
		const std::string& refModelPath,
		const std::string& outputPath
	);

private:
	bool LoadReferenceSkeleton(const std::string& path);
	AnimChannel ConvertAssimpChannelToEngine(aiNodeAnim* srcChannel);
	void BakeAnimation(const aiAnimation* srcAnim, AssetFmt::RawAnimation& outAnim);
	std::string MakeSafeName(const std::string& rawName);
    bool WriteAnimationFile(const std::string& finalPath, const AssetFmt::RawAnimation& anim);

private:
    // m_rawAnim 멤버 변수 제거 (지역 변수 사용)
	std::vector<AssetFmt::RawBone> m_bones;
	std::unordered_map<std::string, int32> m_boneNameMap;
};