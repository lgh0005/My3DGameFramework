#include "EnginePch.h"
#include "Skeleton.h"

DECLARE_DEFAULTS_IMPL(Skeleton)

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

	// 3. 벡터 캐시에 오프셋 행렬 저장 (인덱스 = ID)
	if (m_boneOffsets.size() <= m_boneCounter)
		m_boneOffsets.resize(m_boneCounter + 1);

	m_boneOffsets[m_boneCounter] = offset;

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

	LOG_WARN("Vertex has too many bone influences. Limit is {}", MAX_BONE_INFLUENCE);
}

void Skeleton::SetData(const BoneMap& map, int32 count)
{
	m_boneInfoMap = map;
	m_boneCounter = count;

	// 맵 데이터를 기반으로 벡터 캐시 재구축
	m_boneOffsets.clear(); // 안전하게 비우고 시작
	m_boneOffsets.resize(count, glm::mat4(1.0f));

	for (const auto& [name, info] : m_boneInfoMap)
	{
		if (info.id >= 0 && info.id < count)
			m_boneOffsets[info.id] = info.offset;
	}
}

const Skeleton::BoneMap& Skeleton::GetBoneInfoMap() const { return m_boneInfoMap; }

int32 Skeleton::GetBoneCount() const { return m_boneCounter; }

int32 Skeleton::GetBoneID(const std::string& name) const
{
	auto it = m_boneInfoMap.find(name);
	if (it != m_boneInfoMap.end()) return it->second.id;
	return -1;
}

const glm::mat4& Skeleton::GetBoneOffset(int32 boneID) const
{
	if (boneID >= 0 && boneID < m_boneOffsets.size()) return m_boneOffsets[boneID];
	return glm::mat4(1.0f);
}