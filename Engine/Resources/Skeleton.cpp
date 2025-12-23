#include "EnginePch.h"
#include "Skeleton.h"

SkeletonUPtr Skeleton::Create()
{
	return SkeletonUPtr(new Skeleton());
}

int32 Skeleton::AddBone(const std::string& name, const glm::mat4& offset)
{
	// 1. 이미 등록된 뼈인지 확인
	auto it = m_boneInfoMap.find(name);
	if (it != m_boneInfoMap.end()) return it->second.id;

	// 2. 새로운 뼈 등록
	AssetFmt::RawBoneInfo newBoneInfo;
	newBoneInfo.id = m_boneCounter;
	newBoneInfo.offset = offset;

	m_boneInfoMap[name] = newBoneInfo;

	// ID 반환 후 카운터 증가
	return m_boneCounter++;
}

void Skeleton::InitializeVertexBoneData(SkinnedVertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void Skeleton::AddBoneWeightToVertex(SkinnedVertex& vertex, int32 boneID, float weight)
{
	// 빈 슬롯(-1)을 찾아 웨이트 주입
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.boneIDs[i] < 0)
		{
			vertex.weights[i] = weight;
			vertex.boneIDs[i] = boneID;
			return; // 주입 성공 시 리턴
		}
	}

	SPDLOG_WARN("Vertex has too many bone influences. Limit is {}", MAX_BONE_INFLUENCE);
}

void Skeleton::SetData(const BoneMap& map, int32 count)
{
	m_boneInfoMap = map;
	m_boneCounter = count;
}

const Skeleton::BoneMap& Skeleton::GetBoneInfoMap() const { return m_boneInfoMap; }

int32 Skeleton::GetBoneCount() const { return m_boneCounter; }

int32 Skeleton::GetBoneID(const std::string& name) const
{
	auto it = m_boneInfoMap.find(name);
	if (it != m_boneInfoMap.end()) return it->second.id;
	return -1;
}