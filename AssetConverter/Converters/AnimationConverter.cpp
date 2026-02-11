#include "pch.h"
#include "AnimationConverter.h"

bool AnimationConverter::Convert
(
	const std::string& animPath,
	const std::string& refModelPath, // .mymodel file only
	const std::string& outputPath
)
{
	LOG_INFO(" [AnimConverter] Start Conversion");
	LOG_INFO(" - Source: {}", animPath);
	LOG_INFO(" - Ref Model: {}", refModelPath);

	// 2. 참조 모델(.mymodel) 로드 및 뼈 이름 검증용 Set 구축
	if (!LoadReferenceSkeleton(refModelPath))
	{
		LOG_ERROR("Failed to load reference model nodes.");
		return false;
	}

	// 3. Assimp로 애니메이션 파일 로드
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animPath, 0);
	if (!scene || !scene->mRootNode)
	{
		LOG_ERROR("Assimp failed: {}", importer.GetErrorString());
		return false;
	}

	if (!scene->HasAnimations())
	{
		LOG_WARN("No animations found in file: {}", animPath);
		return false;
	}

	// 4. 애니메이션 클립별로 분리 저장
	int32 animCount = scene->mNumAnimations;
	LOG_INFO("Found {} animations.", animCount);

	// 출력 경로 분해 (예: Assets/Player.myanim -> Dir: Assets, Stem: Player)
	// TODO : 해당 문자열을 따로 멤버로 둘 필요가 있다.
	std::filesystem::path outPathObj(outputPath);
	std::string parentDir = outPathObj.parent_path().string();
	std::string fileStem = outPathObj.stem().string();

	int32 successCount = 0;
	for (int i = 0; i < animCount; ++i)
	{
		aiAnimation* srcAnim = scene->mAnimations[i];
		AssetFmt::RawAnimation rawAnim;

		// 이름을 정제
		rawAnim.name = MakeSafeName(srcAnim->mName.C_Str());
		if (rawAnim.name.empty()) rawAnim.name = "Clip_" + std::to_string(i);

		// 4-1. 파일명 결정 (원본파일명_클립명.myanim)
		BakeAnimation(srcAnim, rawAnim);

		// 4-2. 저장 경로 설정
		std::string finalName = fmt::format("{}_{}.myanim", fileStem, rawAnim.name);
		std::filesystem::path finalPath = std::filesystem::path(parentDir) / finalName;

		// 4-3. 저장 (Write)
		if (WriteAnimationFile(finalPath.string(), rawAnim))
		{
			LOG_INFO("   -> Exported: {}", finalName);
			successCount++;
		}
	}

	return (successCount > 0);
}

bool AnimationConverter::LoadReferenceSkeleton(const std::string& path)
{
	// 1. 초기화
	m_bones.clear();
	m_boneNameMap.clear();
	LOG_INFO("Trying to open: {}", path);

	// 2. 파일 존재 확인
	if (!std::filesystem::exists(path))
	{
		LOG_ERROR("FILE NOT FOUND! Path is wrong.");
		LOG_ERROR("   -> You typed: {}", path);
		return false;
	}

	std::ifstream inFile(path, std::ios::binary);
	if (!inFile)
	{
		LOG_ERROR("File exists but cannot open (Permission denied?)");
		LOG_ERROR("Reason : {}", std::strerror(errno));
		return false;
	}

	// 3. Header 읽기
	uint32 magic = AssetUtils::ReadData<uint32>(inFile);
	if (magic != 0x4D594D44) return false; // MYMD

	AssetUtils::ReadData<uint32>(inFile);					   // version
	AssetUtils::ReadData<uint32>(inFile);					   // matCount
	AssetUtils::ReadData<uint32>(inFile);					   // meshCount
	bool hasSkeleton = AssetUtils::ReadData<bool>(inFile);     // hasSkeleton
	if (!hasSkeleton)
	{
		LOG_ERROR("Selected Reference Model has NO Skeleton data! Cannot bake animation.");
		return false;
	}
	AssetUtils::ReadData<glm::vec3>(inFile);				   // AABB Min
	AssetUtils::ReadData<glm::vec3>(inFile);				   // AABB Max

	// 4. Hierarchy Nodes 읽기
	auto rawNodes = AssetUtils::ReadRawNodes(inFile);
	std::unordered_map<std::string, int32> nodeNameToIndex;
	for (int32 i = 0; i < (int32)rawNodes.size(); ++i)
		nodeNameToIndex[rawNodes[i].name] = i;

	// 5. Skeleton Data 읽기
	uint32 boneCount = AssetUtils::ReadData<uint32>(inFile);

	// 5-1. RawBoneInfo 목록 읽기
	std::vector<AssetFmt::RawBoneInfo> boneInfos(boneCount);
	for (uint32 i = 0; i < boneCount; ++i)
	{
		boneInfos[i].id = AssetUtils::ReadData<uint32>(inFile);
		boneInfos[i].offset = AssetUtils::ReadData<glm::mat4>(inFile);
	}

	// 5-2. Bone Map 읽어서 m_bones 구축
	uint32 mapCount = AssetUtils::ReadData<uint32>(inFile);
	m_bones.resize(boneCount);

	for (uint32 i = 0; i < mapCount; ++i)
	{
		std::string name = AssetUtils::ReadString(inFile);
		int32 id = AssetUtils::ReadData<int32>(inFile);

		if (id >= 0 && id < (int32)boneCount)
		{
			m_bones[id].name = name;
			m_bones[id].index = id;
			m_bones[id].offsetMatrix = boneInfos[id].offset;
			m_boneNameMap[name] = id;
		}
	}

	// 6. 부모 뼈 연결
	for (auto& bone : m_bones)
	{
		auto itNode = nodeNameToIndex.find(bone.name);
		if (itNode == nodeNameToIndex.end()) continue;

		int32 myNodeIdx = itNode->second;
		int32 parentNodeIdx = rawNodes[myNodeIdx].parentIndex;

		if (parentNodeIdx != -1)
		{
			std::string parentNodeName = rawNodes[parentNodeIdx].name;
			auto itParentBone = m_boneNameMap.find(parentNodeName);
			if (itParentBone != m_boneNameMap.end())
				bone.parentIndex = itParentBone->second;
		}
	}

	LOG_INFO(" - Reference Skeleton Loaded: {} bones linked.", boneCount);
	inFile.close();
	return true;
}

AnimChannel AnimationConverter::ConvertAssimpChannelToEngine(aiNodeAnim* srcChannel)
{
	std::string nodeName = srcChannel->mNodeName.C_Str();

	// 1. Position Keys
	std::vector<AssetFmt::RawKeyPosition> positions;
	positions.reserve(srcChannel->mNumPositionKeys);
	for (uint32 i = 0; i < srcChannel->mNumPositionKeys; ++i)
	{
		const auto& k = srcChannel->mPositionKeys[i];
		positions.push_back({ Utils::ConvertToGLMVec(k.mValue), (float)k.mTime });
	}

	// 2. Rotation Keys
	std::vector<AssetFmt::RawKeyRotation> rotations;
	rotations.reserve(srcChannel->mNumRotationKeys);
	for (uint32 i = 0; i < srcChannel->mNumRotationKeys; ++i)
	{
		const auto& k = srcChannel->mRotationKeys[i];
		rotations.push_back({ Utils::ConvertToGLMQuat(k.mValue), (float)k.mTime });
	}

	// 3. Scale Keys
	std::vector<AssetFmt::RawKeyScale> scales;
	scales.reserve(srcChannel->mNumScalingKeys);
	for (uint32 i = 0; i < srcChannel->mNumScalingKeys; ++i)
	{
		const auto& k = srcChannel->mScalingKeys[i];
		scales.push_back({ Utils::ConvertToGLMVec(k.mValue), (float)k.mTime });
	}

	return AnimChannel
	(
		nodeName, 
		std::move(positions), 
		std::move(rotations), 
		std::move(scales)
	);
}

void AnimationConverter::BakeAnimation(const aiAnimation* srcAnim, AssetFmt::RawAnimation& outAnim)
{
	// 1. 설정
	const float FRAME_RATE = 30.0f;
	float durationTicks = (float)srcAnim->mDuration;
	float ticksPerSecond = (srcAnim->mTicksPerSecond != 0) ? (float)srcAnim->mTicksPerSecond : 25.0f;
	float durationSeconds = durationTicks / ticksPerSecond;

	// 헤더 정보 채우기
	outAnim.duration = durationSeconds;
	outAnim.ticksPerSecond = ticksPerSecond;
	outAnim.frameRate = FRAME_RATE;
	outAnim.boneCount = (uint32)m_bones.size();

	// 프레임 수 계산 (올림 처리)
	outAnim.frameCount = (uint32)ceil(durationSeconds * FRAME_RATE) + 1;
	outAnim.bakedMatrices.resize(outAnim.frameCount * outAnim.boneCount);

	// 임시 AnimChannel 맵 생성 (검색 성능 향상)
	std::unordered_map<std::string, AnimChannel> tempChannels;
	for (uint32 i = 0; i < srcAnim->mNumChannels; ++i)
	{
		auto channel = ConvertAssimpChannelToEngine(srcAnim->mChannels[i]);
		tempChannels[channel.GetBoneName()] = std::move(channel);
	}

	// 베이킹 루프
	float timeStepPerFrame = 1.0f / FRAME_RATE;
	for (uint32 f = 0; f < outAnim.frameCount; ++f)
	{
		float timeInSeconds = f * timeStepPerFrame;
		float timeInTicks = timeInSeconds * ticksPerSecond;
		if (timeInTicks > durationTicks) timeInTicks = durationTicks;

		for (int32 b = 0; b < (int32)m_bones.size(); ++b)
		{
			auto& bone = m_bones[b];

			// (1) Local Matrix 계산
			auto itChannel = tempChannels.find(bone.name);
			if (itChannel != tempChannels.end())
				bone.localMatrix = itChannel->second.GetPose(timeInTicks).ToMat4();
			else bone.localMatrix = glm::mat4(1.0f);

			// (2) Global Matrix 계산 (계층 구조 전파)
			if (bone.parentIndex != -1)
				bone.globalMatrix = m_bones[bone.parentIndex].globalMatrix * bone.localMatrix;
			else bone.globalMatrix = bone.localMatrix;

			// (3) 최종 스키닝 행렬 저장 (Global * Offset)
			glm::mat4 finalMat = bone.globalMatrix * bone.offsetMatrix;
			uint32 index = f * outAnim.boneCount + b;
			outAnim.bakedMatrices[index] = finalMat;
		}
	}

	LOG_INFO
	(
		"   - Baked Info: {:.2f} sec, {} frames, {} bones",
		outAnim.duration,
		outAnim.frameCount,
		outAnim.boneCount
	);
}

std::string AnimationConverter::MakeSafeName(const std::string& rawName)
{
	std::string safeName = rawName;

	// 1. 구분자(|, :) 뒤의 진짜 이름만 추출
	usize lastSeparator = safeName.find_last_of("|:");
	if (lastSeparator != std::string::npos)
		safeName = safeName.substr(lastSeparator + 1);

	// 2. 파일 시스템 금지 문자 및 공백 치환
	const std::string invalidChars = "<>:\"/\\|?* ";
	for (char& c : safeName)
	{
		// 금지 문자가 포함되어 있다면 '_'로 변경
		if (invalidChars.find(c) != std::string::npos)
			c = '_';
	}

	// 3. 만약 다 지워서 빈 문자열이 됐다면 기본값 부여
	if (safeName.empty()) safeName = "Unnamed_Anim";

	return safeName;
}

bool AnimationConverter::WriteAnimationFile(const std::string& finalPath, const AssetFmt::RawAnimation& anim)
{
	std::ofstream outFile(finalPath, std::ios::binary);
	if (!outFile) return false;

	AssetUtils::WriteRawAnimation(outFile, anim);

	outFile.close();
	return true;
}

