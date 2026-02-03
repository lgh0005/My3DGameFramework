#include "Animation.h"
#include "Resources/Model.h"
#include "Resources/Animations/AnimChannel.h"
#include "Resources/Animations/Skeleton.h"
#include "Misc/AssetUtils.h"

DECLARE_DEFAULTS_IMPL(Animation)

AnimationUPtr Animation::Load(const std::string& filePath, Model* model)
{
	auto animation = AnimationUPtr(new Animation());

	// 1. 파일 확장명 비교 후 로드 : .myanim으로 로드하는 것을 추천
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
			LOG_ERROR("Model pointer is required for Assimp animation loading: {}", filePath);
			return nullptr;
		}
		if (!animation->LoadByAssimp(filePath, model)) return nullptr;
	}

	return animation;
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

void Animation::Bake(Skeleton* skeleton)
{
	if (!skeleton) return;
	if (m_bakedData.frameCount > 0 && m_bakedData.boneCount > 0) return;

	// 1. 베이킹 설정 (30 FPS 고정)
	m_bakedData.frameRate = 30.0f;
	float timeStep = 1.0f / m_bakedData.frameRate;

	// 전체 프레임 수 계산 (마지막 프레임 포함을 위해 ceil 사용)
	m_bakedData.frameCount = (uint32)ceil(m_duration * m_bakedData.frameRate) + 1;
	m_bakedData.boneCount = (uint32)skeleton->GetBoneCount();

	// 2. 메모리 예약 (전체 크기 = 프레임 수 * 뼈 개수)
	uint32 totalSize = m_bakedData.frameCount * m_bakedData.boneCount;
	m_bakedData.localMatrices.clear();
	m_bakedData.localMatrices.resize(totalSize);

	// 3. 베이킹 루프 (시간 -> 뼈)
	for (uint32 f = 0; f < m_bakedData.frameCount; ++f)
	{
		// 현재 샘플링 시간
		float currentTime = f * timeStep;
		if (currentTime > m_duration) currentTime = m_duration;

		uint32 frameOffset = f * m_bakedData.boneCount;

		for (int32 b = 0; b < (int32)m_bakedData.boneCount; ++b)
		{
			glm::mat4 localMatrix;
			uint32 boneHash = skeleton->GetBoneHash(b);
			AnimChannel* channel = FindChannel(boneHash);
			if (channel)
			{
				// 채널이 있다면: 현재 시간의 포즈를 보간해서 가져옴
				localMatrix = channel->GetPose(currentTime).ToMat4();
			}
			else
			{
				// 채널이 없다면: 움직이지 않는 뼈 (Identity)
				// TODO: 추후 Bind Pose가 필요하다면 Skeleton에서 가져와야 함
				localMatrix = glm::mat4(1.0f);
			}

			// 3-3. 1차원 배열에 저장
			// 인덱스 공식: FrameIndex * BoneCount + BoneID
			m_bakedData.localMatrices[frameOffset + b] = localMatrix;
		}
	}

	LOG_INFO
	(
		"Animation [{}] Baked: {} Frames ({:.2f} sec), {} Bones (Total {} Matrices)",
		m_name,
		m_bakedData.frameCount,
		m_bakedData.frameCount / m_bakedData.frameRate,
		m_bakedData.boneCount,
		totalSize
	);
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
		LOG_ERROR("Failed to load animation file: {}", animationPath);
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
			std::move(rawCh.positions),
			std::move(rawCh.rotations),
			std::move(rawCh.scales)
		);

		uint32 nameHash = Utils::StrHash(boneName);
		m_channelMap[nameHash] = newChannel.get();
		m_channels.push_back(std::move(newChannel));
	}

	inFile.close();
	LOG_INFO("Loaded binary animation: {} ({} channels)", filePath, m_channels.size());
	return true;
}

void Animation::ParseAssimpChannels(const aiAnimation* animation)
{
	uint32 size = animation->mNumChannels;
	m_channels.reserve(size);

	for (uint32 i = 0; i < size; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.C_Str();

		auto newChannel = AnimChannel::Create(boneName, channel);
		m_channelMap[newChannel->GetNameHash()] = newChannel.get();
		m_channels.push_back(std::move(newChannel));
	}
}
