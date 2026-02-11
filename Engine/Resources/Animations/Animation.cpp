#include "Animation.h"
#include "Resources/Model.h"
#include "Resources/Animations/AnimChannel.h"
#include "Resources/Animations/Skeleton.h"
#include "Misc/AssetUtils.h"

DECLARE_DEFAULTS_IMPL(Animation)

AnimationUPtr Animation::Load(const std::string& filePath)
{
	// 1. 파일 확장명 비교 후 로드(.myanim)
	std::string ext = std::filesystem::path(filePath).extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	if (ext != ".myanim") return nullptr;

	// 2. 애니메이션 로드
	auto animation = AnimationUPtr(new Animation());
	if (!animation->LoadByBinary(filePath)) return nullptr;
	return std::move(animation);
}

AnimChannel* Animation::FindChannel(const std::string& name)
{
	return FindChannel(Utils::StrHash(name));
}

AnimChannel* Animation::FindChannel(uint32 nameHash)
{
	auto it = m_channelMap.find(nameHash);
	if (it != m_channelMap.end()) return it->second;
	return nullptr;
}

/*===================================//
//   keyframe load process methods   //
//===================================*/
bool Animation::LoadByBinary(const std::string& filePath)
{
	std::ifstream inFile(filePath, std::ios::binary);
	if (!inFile)
	{
		LOG_ERROR("Failed to open animation file: {}", filePath);
		return false;
	}

	// AssetUtils가 포맷에 맞춰서 구조체로 싹 읽어옴
	AssetFmt::RawAnimation rawAnim = AssetUtils::ReadRawAnimation(inFile);
	if (rawAnim.magic != 0x414E494D)
	{
		LOG_ERROR("Invalid animation file magic.");
		return false;
	}
	inFile.close();

	// 멤버 변수 채우기
	m_name = rawAnim.name;
	m_duration = rawAnim.duration;
	m_ticksPerSecond = rawAnim.ticksPerSecond;
	m_animClip.frameRate = rawAnim.frameRate;
	m_animClip.frameCount = rawAnim.frameCount;
	m_animClip.boneCount = rawAnim.boneCount;
	m_animClip.localMatrices = std::move(rawAnim.bakedMatrices);

	// 4. 채널 정보 로드 (CPU 로직용)
	m_channels.reserve(rawAnim.channels.size());
	for (auto& rawCh : rawAnim.channels)
	{
		std::string boneName = rawCh.nodeName;
		auto newChannel = AnimChannel::Create
		(
			boneName,
			std::move(rawCh.positions),
			std::move(rawCh.rotations),
			std::move(rawCh.scales)
		);

		uint32 nameHash = Utils::StrHash(boneName);
		m_channelMap[nameHash] = newChannel.get();
		m_channels.push_back(std::move(newChannel));
	}

	// 5. 검증
	if (m_animClip.IsValid())
	{
		LOG_INFO
		(
			"Loaded Baked Animation: {} ({} frames, {} bones)",
			m_name, m_animClip.frameCount, m_animClip.boneCount
		);
	}
	else
	{
		LOG_ERROR("Loaded animation [{}] but AnimClip is empty/invalid.", m_name);
		return false;
	}

	return true;
}
