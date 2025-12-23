#include "Animation.h"
#include "Resources/Model.h"
#include "Resources/AnimChannel.h"

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

	ReadMissingBones(animation, *model);

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

	uint32 magic, version;
	inFile.read((char*)&magic, sizeof(magic));
	inFile.read((char*)&version, sizeof(version));

	if (magic != 0x414E494D) // 'ANIM'
	{
		SPDLOG_ERROR("Invalid animation file magic: {}", magic);
		return false;
	}

	// 애니메이션 기본 정보
	uint32 nameLen;
	inFile.read((char*)&nameLen, sizeof(nameLen));
	std::string animName(nameLen, '\0');
	inFile.read(&animName[0], nameLen);

	inFile.read((char*)&m_duration, sizeof(m_duration));
	inFile.read((char*)&m_ticksPerSecond, sizeof(m_ticksPerSecond));

	// 채널 데이터
	uint32 channelCount;
	inFile.read((char*)&channelCount, sizeof(channelCount));

	for (uint32 i = 0; i < channelCount; ++i)
	{
		uint32 boneID, posCount, rotCount, sclCount;
		inFile.read((char*)&boneID, sizeof(boneID));
		inFile.read((char*)&posCount, sizeof(posCount));
		inFile.read((char*)&rotCount, sizeof(rotCount));
		inFile.read((char*)&sclCount, sizeof(sclCount));

		std::vector<AssetFmt::RawKeyPosition> positions(posCount);
		std::vector<AssetFmt::RawKeyRotation> rotations(rotCount);
		std::vector<AssetFmt::RawKeyScale> scales(sclCount);

		if (posCount > 0) inFile.read((char*)positions.data(), sizeof(AssetFmt::RawKeyPosition) * posCount);
		if (rotCount > 0) inFile.read((char*)rotations.data(), sizeof(AssetFmt::RawKeyRotation) * rotCount);
		if (sclCount > 0) inFile.read((char*)scales.data(), sizeof(AssetFmt::RawKeyScale) * sclCount);

		// Bone::Create가 이제 vector를 직접 받도록 수정되었다고 가정
		// m_bones가 std::vector<BoneUPtr>라면 ID에 맞춰 resize 필요할 수도 있음.
		// 여기서는 m_bones에 단순히 push_back 한다고 가정 (Bone 클래스가 ID를 가지고 있으므로)
		m_bones.push_back(AnimChannel::Create(boneID, std::move(positions), std::move(rotations), std::move(scales)));
	}

	inFile.close();
	SPDLOG_INFO("Loaded binary animation: {} ({} channels)", filePath, m_bones.size());
	return true;
}


AnimChannel* Animation::FindBone(const std::string& name)
{
	auto it = std::find_if
	(
		m_bones.begin(), m_bones.end(),
		[&](const AnimChannelUPtr& bonePtr) { return bonePtr->GetBoneName() == name; }
	);
	if (it == m_bones.end()) return nullptr;
	else return it->get();
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	uint32 size = animation->mNumChannels;
	auto& boneInfoMap = model.GetBoneInfoMap();
	int32& boneCount = model.GetBoneCount();

	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.C_Str();

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneInfoMap[boneName].offset = glm::mat4(1.0f);
			boneCount++;
		}
		m_bones.push_back(AnimChannel::Create
		(
			boneName,
			boneInfoMap[boneName].id, channel)
		);
	}

	m_boneInfoMap = boneInfoMap;
}
