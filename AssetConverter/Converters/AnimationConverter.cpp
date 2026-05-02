#include "ConverterPch.h"
#include "AnimationConverter.h"
#include "Managers/ThreadManager.h"

namespace MGF3D
{
	AnimationConverter::AnimationConverter() = default;
	AnimationConverter::~AnimationConverter() = default;

	bool AnimationConverter::Convert
	(
		const String& animPath,
		const String& refModelPath, // .mymodel file only
		const String& outputPath
	)
	{
		MGF_LOG_INFO(" [AnimConverter] Start Conversion");
		MGF_LOG_INFO(" - Source: {}", animPath);
		MGF_LOG_INFO(" - Ref Model: {}", refModelPath);

		// 2. 참조 모델(.mymodel) 로드 및 뼈 이름 검증용 Set 구축
		if (!LoadReferenceSkeleton(refModelPath))
		{
			MGF_LOG_ERROR("Failed to load reference model nodes.");
			return false;
		}

		// 3. Assimp로 애니메이션 파일 로드
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animPath, 0);
		if (!scene || !scene->mRootNode)
		{
			MGF_LOG_ERROR("Assimp failed: {}", importer.GetErrorString());
			return false;
		}

		if (!scene->HasAnimations())
		{
			MGF_LOG_WARN("No animations found in file: {}", animPath);
			return false;
		}

		// 4. 애니메이션 클립별로 분리 저장
		int32 animCount = scene->mNumAnimations;
		MGF_LOG_INFO("Found {} animations.", animCount);

		// 출력 경로 분해 (예: Assets/Player.myanim -> Dir: Assets, Stem: Player)
		// TODO : 해당 문자열을 따로 멤버로 둘 필요가 있다.
		FileSystem::path outPathObj(outputPath);
		String parentDir = outPathObj.parent_path().string();
		String fileStem = outPathObj.stem().string();

		m_successCount = 0;
		for (int i = 0; i < animCount; ++i)
		{
			aiAnimation* srcAnim = scene->mAnimations[i];

			AddTaskCount();
			MGF_THREAD.PushTask
			(
				[this, srcAnim, parentDir, fileStem, i]()
				{
					RawAnimation rawAnim;
					rawAnim.name = MakeSafeName(srcAnim->mName.C_Str());
					if (rawAnim.name.empty()) rawAnim.name = "Clip_" + std::to_string(i);

					// 1. 베이킹
					BakeAnimation(srcAnim, rawAnim);

					// 2. 파일명 결정
					String finalName = fmt::format("{}_{}.myanim", fileStem, rawAnim.name);
					FileSystem::path finalPath = FileSystem::path(parentDir) / finalName;

					// 3. 파일 쓰기
					if (WriteAnimationFile(finalPath.string(), rawAnim))
					{
						MGF_LOG_INFO("   -> Exported: {}", finalName);
						++m_successCount;
					}

					CompleteTaskCount();
				}
			);
		}

		WaitAllTasks();
		return (m_successCount.load() > 0);
	}

	bool AnimationConverter::LoadReferenceSkeleton(const String& path)
	{
		// 1. 초기화
		m_bones.clear();
		m_boneNameMap.clear();
		MGF_LOG_INFO("Trying to open: {}", path);

		// 2. 파일 존재 확인
		if (!FileSystem::exists(path))
		{
			MGF_LOG_ERROR("FILE NOT FOUND! Path is wrong.");
			MGF_LOG_ERROR("   -> You typed: {}", path);
			return false;
		}

		InputFileStream inFile(path, std::ios::binary);
		if (!inFile)
		{
			MGF_LOG_ERROR("File exists but cannot open (Permission denied?)");
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
			MGF_LOG_ERROR("Selected Reference Model has NO Skeleton data! Cannot bake animation.");
			return false;
		}
		AssetUtils::ReadData<vec3>(inFile);				   // AABB Min
		AssetUtils::ReadData<vec3>(inFile);				   // AABB Max

		// 4. Hierarchy Nodes 읽기
		auto rawNodes = AssetUtils::ReadRawNodes(inFile);
		std::unordered_map<String, int32> nodeNameToIndex;
		for (int32 i = 0; i < (int32)rawNodes.size(); ++i)
			nodeNameToIndex[rawNodes[i].name] = i;

		// 5. Skeleton Data 읽기
		uint32 boneCount = AssetUtils::ReadData<uint32>(inFile);

		// 5-1. RawBoneInfo 목록 읽기
		Vector<RawBoneInfo> boneInfos(boneCount);
		for (uint32 i = 0; i < boneCount; ++i)
		{
			boneInfos[i].id = AssetUtils::ReadData<uint32>(inFile);
			boneInfos[i].offset = AssetUtils::ReadData<mat4>(inFile);
		}

		// 5-2. Bone Map 읽어서 m_bones 구축
		uint32 mapCount = AssetUtils::ReadData<uint32>(inFile);
		m_bones.resize(boneCount);

		for (uint32 i = 0; i < mapCount; ++i)
		{
			String name = AssetUtils::ReadString(inFile);
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
				String parentNodeName = rawNodes[parentNodeIdx].name;
				auto itParentBone = m_boneNameMap.find(parentNodeName);
				if (itParentBone != m_boneNameMap.end())
					bone.parentIndex = itParentBone->second;
			}
		}

		MGF_LOG_INFO(" - Reference Skeleton Loaded: {} bones linked.", boneCount);
		inFile.close();
		return true;
	}

	AnimChannel AnimationConverter::ConvertAssimpChannelToEngine(aiNodeAnim* srcChannel)
	{
		String nodeName = srcChannel->mNodeName.C_Str();

		// 1. Position Keys
		Vector<RawKeyPosition> positions;
		positions.reserve(srcChannel->mNumPositionKeys);
		for (uint32 i = 0; i < srcChannel->mNumPositionKeys; ++i)
		{
			const auto& k = srcChannel->mPositionKeys[i];
			positions.push_back({ Math::ConvertToGLMVec(k.mValue), (float)k.mTime });
		}

		// 2. Rotation Keys
		Vector<RawKeyRotation> rotations;
		rotations.reserve(srcChannel->mNumRotationKeys);
		for (uint32 i = 0; i < srcChannel->mNumRotationKeys; ++i)
		{
			const auto& k = srcChannel->mRotationKeys[i];
			rotations.push_back({ Math::ConvertToGLMQuat(k.mValue), (float)k.mTime });
		}

		// 3. Scale Keys
		Vector<RawKeyScale> scales;
		scales.reserve(srcChannel->mNumScalingKeys);
		for (uint32 i = 0; i < srcChannel->mNumScalingKeys; ++i)
		{
			const auto& k = srcChannel->mScalingKeys[i];
			scales.push_back({ Math::ConvertToGLMVec(k.mValue), (float)k.mTime });
		}

		return AnimChannel
		(
			nodeName,
			std::move(positions),
			std::move(rotations),
			std::move(scales)
		);
	}

	void AnimationConverter::BakeAnimation(const aiAnimation* srcAnim, RawAnimation& outAnim)
	{
		// 1. 설정
		const float FRAME_RATE = 30.0f;
		float durationTicks = (float)srcAnim->mDuration;
		float ticksPerSecond = (srcAnim->mTicksPerSecond != 0) ? (float)srcAnim->mTicksPerSecond : 25.0f;
		float durationSeconds = durationTicks / ticksPerSecond;

		// 헤더 정보 채우기
		outAnim.magic = 0x414E494D; // "ANIM"
		outAnim.version = 2;
		outAnim.duration = durationSeconds;
		outAnim.ticksPerSecond = ticksPerSecond;
		outAnim.frameRate = FRAME_RATE;
		outAnim.boneCount = (uint32)m_bones.size();
		if (outAnim.boneCount == 0)
		{
			MGF_LOG_ERROR("Cannot bake animation: Reference Skeleton has 0 bones!");
			return;
		}

		// 2. 채널 데이터를 Raw 포맷으로 변환하여 저장 (엔진 런타임 보간용)
		outAnim.channels.reserve(srcAnim->mNumChannels);
		for (uint32 i = 0; i < srcAnim->mNumChannels; ++i)
		{
			aiNodeAnim* srcCh = srcAnim->mChannels[i];
			RawAnimChannel rawCh;
			rawCh.nodeName = srcCh->mNodeName.C_Str();

			// 위치 키프레임
			for (uint32 k = 0; k < srcCh->mNumPositionKeys; ++k)
				rawCh.positions.push_back({ Math::ConvertToGLMVec(srcCh->mPositionKeys[k].mValue), (float)srcCh->mPositionKeys[k].mTime });

			// 회전 키프레임
			for (uint32 k = 0; k < srcCh->mNumRotationKeys; ++k)
				rawCh.rotations.push_back({ Math::ConvertToGLMQuat(srcCh->mRotationKeys[k].mValue), (float)srcCh->mRotationKeys[k].mTime });

			// 스케일 키프레임
			for (uint32 k = 0; k < srcCh->mNumScalingKeys; ++k)
				rawCh.scales.push_back({ Math::ConvertToGLMVec(srcCh->mScalingKeys[k].mValue), (float)srcCh->mScalingKeys[k].mTime });

			outAnim.channels.push_back(std::move(rawCh));
		}

		// 3. 베이킹 준비 (행렬 계산을 위해 엔진용 AnimChannel 객체 활용)
		HashMap<String, AnimChannel> bakeHelpers;
		for (auto& rawCh : outAnim.channels)
		{
			// 복사본을 만들어 베이킹 헬퍼로 사용 (원본은 outAnim에 보존)
			bakeHelpers[rawCh.nodeName] = AnimChannel
			(
				rawCh.nodeName,
				Vector<RawKeyPosition>(rawCh.positions),
				Vector<RawKeyRotation>(rawCh.rotations),
				Vector<RawKeyScale>(rawCh.scales)
			);
		}

		outAnim.frameCount = (uint32)ceil(durationSeconds * FRAME_RATE) + 1;
		outAnim.bakedMatrices.resize(outAnim.frameCount * outAnim.boneCount);

		// 베이킹 루프
		float timeStepPerFrame = 1.0f / FRAME_RATE;
		Vector<mat4> localMatrices(m_bones.size());
		Vector<mat4> globalMatrices(m_bones.size());
		for (uint32 f = 0; f < outAnim.frameCount; ++f)
		{
			float timeInSeconds = f * timeStepPerFrame;
			float timeInTicks = timeInSeconds * ticksPerSecond;
			if (timeInTicks > durationTicks) timeInTicks = durationTicks;

			for (int32 b = 0; b < (int32)m_bones.size(); ++b)
			{
				const auto& refBone = m_bones[b];

				// (1) Local Matrix 계산
				auto itChannel = bakeHelpers.find(refBone.name);
				if (itChannel != bakeHelpers.end())
					localMatrices[b] = itChannel->second.GetPose(timeInTicks).ToMat4();
				else localMatrices[b] = mat4(1.0f);

				// (2) Global Matrix 계산 (계층 구조 전파)
				if (refBone.parentIndex != -1)
					globalMatrices[b] = globalMatrices[refBone.parentIndex] * localMatrices[b];
				else globalMatrices[b] = localMatrices[b];

				// (3) 최종 스키닝 행렬 저장 (Global * Offset)
				uint32 index = f * outAnim.boneCount + b;
				outAnim.bakedMatrices[index] = globalMatrices[b] * refBone.offsetMatrix;
			}
		}

		MGF_LOG_INFO
		(
			"   - Baked Info: {:.2f} sec, {} frames, {} bones",
			outAnim.duration,
			outAnim.frameCount,
			outAnim.boneCount
		);
	}

	String AnimationConverter::MakeSafeName(const String& rawName)
	{
		String safeName = rawName;

		// 1. 구분자(|, :) 뒤의 진짜 이름만 추출
		usize lastSeparator = safeName.find_last_of("|:");
		if (lastSeparator != String::npos)
			safeName = safeName.substr(lastSeparator + 1);

		// 2. 파일 시스템 금지 문자 및 공백 치환
		const String invalidChars = "<>:\"/\\|?* ";
		for (char& c : safeName)
		{
			// 금지 문자가 포함되어 있다면 '_'로 변경
			if (invalidChars.find(c) != String::npos)
				c = '_';
		}

		// 3. 만약 다 지워서 빈 문자열이 됐다면 기본값 부여
		if (safeName.empty()) safeName = "Unnamed_Anim";

		return safeName;
	}

	bool AnimationConverter::WriteAnimationFile(const String& finalPath, const RawAnimation& anim)
	{
		OutputFileStream outFile(finalPath, std::ios::binary);
		if (!outFile) return false;

		AssetUtils::WriteRawAnimation(outFile, anim);

		outFile.close();
		return true;
	}

	/*=============================//
	//   multi-threading members   //
	//=============================*/
	void AnimationConverter::AddTaskCount()
	{
		++m_activeTasks;
	}

	void AnimationConverter::CompleteTaskCount()
	{
		--m_activeTasks;
		if (m_activeTasks == 0)  m_taskCv.notify_all();
	}

	void AnimationConverter::WaitAllTasks()
	{
		UniqueLock lock(m_taskMutex);
		m_taskCv.wait(lock, [this]() { return m_activeTasks.load() == 0; });
	}
}

