#include "../pch.h"
#include "AnimConverter.h"

/*=========================//
//  converter entry point  //
//=========================*/
bool AnimConverter::Convert(const std::string& animPath,
                            const std::string& modelPath,
                            const std::string& outputPath)
{
    AnimConverter converter;
    SPDLOG_INFO("Creating AnimConverter instance and starting conversion...");
    return converter.RunConversion(animPath, modelPath, outputPath);
}

/*===========================//
//  main conversion methods  //
//===========================*/
bool AnimConverter::RunConversion(const std::string& animPath,
                                  const std::string& modelPath,
                                  const std::string& outputPath)
{
    // 1. 기준 스켈레톤 로드 (.mymodel 파일에서)
    SPDLOG_INFO("Loading skeleton from model file: {}", modelPath);
    if (!LoadSkeletonFromMyModel(modelPath))
    {
        SPDLOG_ERROR("Failed to load skeleton from: {}", modelPath);
        return false;
    }
    SPDLOG_INFO("Skeleton loaded. Total {} bones.", m_boneNameToIdMap.size());

    // 2. Assimp로 애니메이션 파일 로드
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animPath, aiProcess_Triangulate);
    if (!scene || !scene->mRootNode)
    {
        SPDLOG_ERROR("Assimp load failed for animation: {}", importer.GetErrorString());
        return false;
    }

    if (!scene->mAnimations || scene->mNumAnimations == 0)
    {
        SPDLOG_ERROR("No animations found in file: {}", animPath);
        return false;
    }

    // 3. 첫 번째 애니메이션만 처리 (여러 개라면 루프 필요)
    SPDLOG_INFO("Processing animation '0' from file...");
    ProcessAnimation(scene->mAnimations[0]);
    SPDLOG_INFO("Processed {} bone channels.", m_boneChannels.size());

    // 4. 커스텀 애니메이션 파일 쓰기
    SPDLOG_INFO("Writing custom animation file: {}", outputPath);
    if (!WriteCustomAnimFile(outputPath))
    {
        SPDLOG_ERROR("Failed to write custom animation file: {}", outputPath);
        return false;
    }

    SPDLOG_INFO("Animation conversion successful!");
    return true;
}

/*===================//
//   load skeleton   //
//===================*/
bool AnimConverter::LoadSkeletonFromMyModel(const std::string& modelPath)
{
    std::ifstream inFile(modelPath, std::ios::binary);
    if (!inFile) return false;

    // 1. 헤더 읽기 (ModelConverter가 쓴 순서대로!)
    uint32 magic, version, materialCount, meshCount;
    bool hasSkeleton;
    inFile.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    inFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    inFile.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
    inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
    inFile.read(reinterpret_cast<char*>(&hasSkeleton), sizeof(hasSkeleton));

    if (!hasSkeleton)
    {
        SPDLOG_ERROR("Referenced model has no skeleton!");
        return false;
    }

    // 2. 스켈레톤 블록 읽기
    uint32 boneCount;
    inFile.read(reinterpret_cast<char*>(&boneCount), sizeof(boneCount));

    for (uint32 i = 0; i < boneCount; ++i)
    {
        // 뼈 이름 읽기
        uint32 nameLen;
        inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string boneName(nameLen, '\0');
        inFile.read(&boneName[0], nameLen);

        // BoneInfo 읽기 (ID와 Offset이 들어있음)
        BoneInfo boneInfo;
        inFile.read(reinterpret_cast<char*>(&boneInfo), sizeof(BoneInfo));

        // [핵심] 이름 <-> ID 맵핑 저장
        m_boneNameToIdMap[boneName] = boneInfo.id;
    }

    inFile.close();
    return true;
}

/*=========================//
//   extract assimp data   //
//=========================*/
void AnimConverter::ProcessAnimation(const aiAnimation* anim)
{
    m_animName = anim->mName.C_Str();
    m_duration = (float)anim->mDuration;
    m_ticksPerSecond = (float)anim->mTicksPerSecond;
    if (m_ticksPerSecond == 0.0f) m_ticksPerSecond = 25.0f; // 기본값

    for (uint32 i = 0; i < anim->mNumChannels; ++i)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();

        // [핵심] 이 채널이 우리 스켈레톤에 있는 뼈인지 확인!
        if (m_boneNameToIdMap.find(boneName) == m_boneNameToIdMap.end())
        {
            // 스켈레톤에 없는 뼈의 애니메이션은 무시합니다. (또는 경고 로그)
            // SPDLOG_WARN("Animation channel for unknown bone: {}", boneName);
            continue;
        }

        // 유효한 뼈라면 처리
        TempBoneChannel tempChannel = ProcessBoneChannel(channel);
        tempChannel.boneID = m_boneNameToIdMap[boneName]; // ID 할당
        m_boneChannels.push_back(std::move(tempChannel));
    }
}

TempBoneChannel AnimConverter::ProcessBoneChannel(const aiNodeAnim* channel)
{
    TempBoneChannel tempChannel;
    tempChannel.boneName = channel->mNodeName.C_Str();

    // 1. 포지션 키 복사
    for (uint32 i = 0; i < channel->mNumPositionKeys; ++i)
    {
        aiVector3D pos = channel->mPositionKeys[i].mValue;
        float time = (float)channel->mPositionKeys[i].mTime;
        tempChannel.positions.push_back({ glm::vec3(pos.x, pos.y, pos.z), time });
    }

    // 2. 로테이션 키 복사
    for (uint32 i = 0; i < channel->mNumRotationKeys; ++i)
    {
        aiQuaternion rot = channel->mRotationKeys[i].mValue;
        float time = (float)channel->mRotationKeys[i].mTime;
        tempChannel.rotations.push_back({ glm::quat(rot.w, rot.x, rot.y, rot.z), time });
    }

    // 3. 스케일 키 복사
    for (uint32 i = 0; i < channel->mNumScalingKeys; ++i)
    {
        aiVector3D scale = channel->mScalingKeys[i].mValue;
        float time = (float)channel->mScalingKeys[i].mTime;
        tempChannel.scales.push_back({ glm::vec3(scale.x, scale.y, scale.z), time });
    }

    return tempChannel;
}

/*=================================//
//   write custom animation file   //
//=================================*/
bool AnimConverter::WriteCustomAnimFile(const std::string& outputPath)
{
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) return false;

    // 1. 헤더
    uint32 magic = 0x414E494D; // 'ANIM'
    uint32 version = 1;
    ConverterUtils::WriteData(outFile, magic);
    ConverterUtils::WriteData(outFile, version);

    // 2. 애니메이션 기본 정보
    ConverterUtils::WriteString(outFile, m_animName);
    ConverterUtils::WriteData(outFile, m_duration);
    ConverterUtils::WriteData(outFile, m_ticksPerSecond);

    // 3. 채널 데이터
    uint32 channelCount = (uint32)m_boneChannels.size();
    ConverterUtils::WriteData(outFile, channelCount);

    for (const auto& channel : m_boneChannels)
    {
        // 채널 헤더 (어떤 뼈인지)
        ConverterUtils::WriteData(outFile, channel.boneID);
        // WriteString(outFile, channel.boneName); // 디버깅용으로 이름도 저장 가능

        // 키프레임 개수
        uint32 posCount = (uint32)channel.positions.size();
        uint32 rotCount = (uint32)channel.rotations.size();
        uint32 sclCount = (uint32)channel.scales.size();
        ConverterUtils::WriteData(outFile, posCount);
        ConverterUtils::WriteData(outFile, rotCount);
        ConverterUtils::WriteData(outFile, sclCount);

        // 키프레임 데이터 (덩어리로 쓰기)
        if (posCount > 0) outFile.write(reinterpret_cast<const char*>(channel.positions.data()), sizeof(TempKeyPosition) * posCount);
        if (rotCount > 0) outFile.write(reinterpret_cast<const char*>(channel.rotations.data()), sizeof(TempKeyRotation) * rotCount);
        if (sclCount > 0) outFile.write(reinterpret_cast<const char*>(channel.scales.data()), sizeof(TempKeyScale) * sclCount);
    }

    outFile.close();
    return true;
}