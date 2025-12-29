#pragma once

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
	void ProcessChannel(aiNodeAnim* srcChannel, AssetFmt::RawAnimChannel& dstChannel);
	bool LoadReferenceSkeleton(const std::string& path);
    bool ProcessSingleClip(aiAnimation* srcAnim, AssetFmt::RawAnimation& outAnim);
	std::string MakeSafeName(const std::string& rawName);
    bool WriteAnimationFile(const std::string& finalPath, const AssetFmt::RawAnimation& anim);

private:
    // m_rawAnim 멤버 변수 제거 (지역 변수 사용)
    std::unordered_set<std::string> m_validNodeNames;
};