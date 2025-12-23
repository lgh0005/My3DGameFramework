#include "pch.h"
#include "AnimationConverter.h"

bool AnimationConverter::Convert
(
	const std::string& animPath,
	const std::string& refModelPath, // .mymodel file only
	const std::string& outputPath
)
{
	// 1. 초기화
	m_validNodeNames.clear();

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
	const aiScene* scene = importer.ReadFile(animPath, aiProcess_LimitBoneWeights);
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

		// 4-1. 변환 (Convert)
		if (!ProcessSingleClip(srcAnim, rawAnim)) continue;

		// 4-2. 파일명 결정 (원본파일명_클립명.myanim)
		// 예: Player_Idle.myanim, Player_Run.myanim
		std::string safeAnimName = rawAnim.name;
		// (필요하다면 여기서 공백이나 특수문자를 _로 치환하는 로직 추가)

		std::string finalName = fmt::format("{}_{}.myanim", fileStem, safeAnimName);
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
	std::ifstream inFile(path, std::ios::binary);
	if (!inFile) return false;

	// 1. 헤더 검증
	uint32_t magic = AssetUtils::ReadData<uint32_t>(inFile);
	if (magic != 0x4D594D44) return false; // MYMD

	uint32 version = AssetUtils::ReadData<uint32>(inFile);

	// 2. 불필요한 정보 건너뛰기 (읽어서 버림)
	AssetUtils::ReadData<uint32_t>(inFile); // matCount
	AssetUtils::ReadData<uint32_t>(inFile); // meshCount
	AssetUtils::ReadData<bool>(inFile);     // hasSkeleton
	AssetUtils::ReadData<glm::vec3>(inFile); // AABB Min
	AssetUtils::ReadData<glm::vec3>(inFile); // AABB Max

	// 3. RawNode 리스트 읽기
	auto nodes = AssetUtils::ReadRawNodes(inFile);

	// 4. 유효 노드 이름 등록
	for (const auto& node : nodes) m_validNodeNames.insert(node.name);

	LOG_INFO(" - Reference Model Valid Nodes: {}", m_validNodeNames.size());
	inFile.close();
	return true;
}

bool AnimationConverter::ProcessSingleClip(aiAnimation* srcAnim, AssetFmt::RawAnimation& outAnim)
{
	std::string animName = srcAnim->mName.C_Str();
	if (animName.empty()) animName = "Anim";

	outAnim.name = MakeSafeName(animName);
	outAnim.duration = (float)srcAnim->mDuration;
	outAnim.ticksPerSecond = (srcAnim->mTicksPerSecond != 0.0) ? (float)srcAnim->mTicksPerSecond : 25.0f;

	// 채널 순회
	for (uint32 i = 0; i < srcAnim->mNumChannels; ++i)
	{
		aiNodeAnim* srcChannel = srcAnim->mChannels[i];
		std::string nodeName = srcChannel->mNodeName.C_Str();

		// 참조 모델에 존재하는 노드인지 체크
		if (m_validNodeNames.find(nodeName) == m_validNodeNames.end()) continue;

		// 유효하다면 채널 추가
		AssetFmt::RawAnimChannel dstChannel;
		ProcessChannel(srcChannel, dstChannel);
		outAnim.channels.push_back(std::move(dstChannel));
	}

	// 채널이 하나도 없으면 실패 처리
	return !outAnim.channels.empty();
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
		{
			c = '_';
		}
	}

	// 3. 만약 다 지워서 빈 문자열이 됐다면 기본값 부여
	if (safeName.empty()) safeName = "Unnamed_Anim";

	return safeName;
}

void AnimationConverter::ProcessChannel(aiNodeAnim* srcChannel, AssetFmt::RawAnimChannel& dstChannel)
{
	dstChannel.nodeName = srcChannel->mNodeName.C_Str();

	// 1. Position
	dstChannel.positions.resize(srcChannel->mNumPositionKeys);
	for (uint32 i = 0; i < srcChannel->mNumPositionKeys; ++i)
	{
		const auto& k = srcChannel->mPositionKeys[i];
		dstChannel.positions[i] = 
		{
			glm::vec3(k.mValue.x, k.mValue.y, k.mValue.z),
			(float)k.mTime
		};
	}

	// 2. Rotation
	dstChannel.rotations.resize(srcChannel->mNumRotationKeys);
	for (uint32 i = 0; i < srcChannel->mNumRotationKeys; ++i)
	{
		const auto& k = srcChannel->mRotationKeys[i];
		// Assimp (w, x, y, z) -> GLM Quat 생성자 (w, x, y, z) 확인 필수!
		// GLM 버전에 따라 quat(w, x, y, z) 또는 quat(x, y, z, w)일 수 있음.
		// 일반적으로 glm::quat(w, x, y, z) 입니다.
		dstChannel.rotations[i] = 
		{
			glm::quat((float)k.mValue.w, (float)k.mValue.x, (float)k.mValue.y, (float)k.mValue.z),
			(float)k.mTime
		};
	}

	// 3. Scale
	dstChannel.scales.resize(srcChannel->mNumScalingKeys);
	for (uint32_t i = 0; i < srcChannel->mNumScalingKeys; ++i)
	{
		const auto& k = srcChannel->mScalingKeys[i];
		dstChannel.scales[i] = 
		{
			glm::vec3(k.mValue.x, k.mValue.y, k.mValue.z),
			(float)k.mTime
		};
	}
}


bool AnimationConverter::WriteAnimationFile(const std::string& finalPath, const AssetFmt::RawAnimation& anim)
{
	std::ofstream outFile(finalPath, std::ios::binary);
	if (!outFile) return false;

	AssetUtils::WriteRawAnimation(outFile, anim);

	outFile.close();
	return true;
}

