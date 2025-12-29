#include "Animation.h"
#include "Resources/Model.h"
#include "Resources/AnimChannel.h"
#include "Resources/Skeleton.h"
#include "Misc/AssetUtils.h"

Animation::Animation() = default;
Animation::~Animation() = default;

AnimationUPtr Animation::Load(const std::string& filePath, Model* model)
{
	auto animation = AnimationUPtr(new Animation());

	// 파일 확장명 비교 후 로드 : .myanim으로 로드하는 것을 추천
	std::string ext = std::filesystem::path(filePath).extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	if (ext == ".myanim")
	{
		if (!animation->LoadByBinary(filePath)) return nullptr;
	}
	else
	{
		// Assimp 로드 시에는 Model 포인터가 필수
		if (!model)
		{
			SPDLOG_ERROR("Model pointer is required for Assimp animation loading: {}", filePath);
			return nullptr;
		}
		if (!animation->LoadByAssimp(filePath, model)) return nullptr;
	}

	return animation;
}

AnimChannel* Animation::FindChannel(const std::string& name)
{
	auto it = m_channelMap.find(name);
	if (it != m_channelMap.end()) return it->second;
	return nullptr;
}

/*===================================//
//   keyframe load process methods   //
//===================================*/
bool Animation::LoadByAssimp(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_LimitBoneWeights);
	if (!scene || !scene->mRootNode)
	{
		SPDLOG_ERROR("Failed to load animation file: {}", animationPath);
		return false;
	}

	aiAnimation* animation = scene->mAnimations[0];
	m_name = animation->mName.C_Str();
	m_duration = (float)animation->mDuration;
	m_ticksPerSecond = (float)animation->mTicksPerSecond;

	ParseAssimpChannels(animation);

	return true;
}

bool Animation::LoadByBinary(const std::string& filePath)
{
	std::ifstream inFile(filePath, std::ios::binary);
	if (!inFile)
	{
		SPDLOG_ERROR("Failed to open animation file: {}", filePath);
		return false;
	}

	// AssetUtils가 포맷에 맞춰서 구조체로 싹 읽어옴
	AssetFmt::RawAnimation rawAnim = AssetUtils::ReadRawAnimation(inFile);
	if (rawAnim.magic != 0x414E494D)
	{
		SPDLOG_ERROR("Invalid animation file magic.");
		return false;
	}

	// 멤버 변수 채우기
	m_name = rawAnim.name;
	m_duration = rawAnim.duration;
	m_ticksPerSecond = rawAnim.ticksPerSecond;

	// 채널 변환 (Raw -> Runtime)
	m_channels.reserve(rawAnim.channels.size());
	for (auto& rawCh : rawAnim.channels)
	{
		std::string boneName = rawCh.nodeName;

		auto newChannel = AnimChannel::Create
		(
			boneName,
			-1,
			std::move(rawCh.positions),
			std::move(rawCh.rotations),
			std::move(rawCh.scales)
		);

		m_channelMap[boneName] = newChannel.get();
		m_channels.push_back(std::move(newChannel));
	}

	inFile.close();
	SPDLOG_INFO("Loaded binary animation: {} ({} channels)", filePath, m_channels.size());
	return true;
}

void Animation::ParseAssimpChannels(const aiAnimation* animation)
{
	uint32 size = animation->mNumChannels;
	for (uint32 i = 0; i < size; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.C_Str();

		// ID는 -1로 설정 (이름으로 매핑하므로 ID 불필요)
		auto newChannel = AnimChannel::Create(boneName, -1, channel);

		m_channelMap[boneName] = newChannel.get();
		m_channels.push_back(std::move(newChannel));
	}
}
