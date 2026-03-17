#include "GraphicsPch.h"
#include "Animation/Skeleton.h"

namespace MGF3D
{
	Skeleton::Skeleton() = default;
	Skeleton::~Skeleton() = default;

	int32 Skeleton::AddBone(const SString& name, const glm::mat4& offset)
	{
		// 0. 뼈 이름 해시값
		StringHash nameHash = name.Hash();

		// 1. 이미 등록된 뼈인지 확인
		auto infoPtr = m_boneInfoMap.Find(nameHash);
		if (infoPtr != nullptr) return infoPtr->id;

		// 2. 새로운 뼈 등록
		AssetFmt::RawBoneInfo newBoneInfo;
		newBoneInfo.id = m_boneCounter;
		newBoneInfo.offset = offset;
		m_boneInfoMap[nameHash] = newBoneInfo;

		// 3. 벡터 캐시에 오프셋 행렬 및 Hash ID 캐시 저장 (인덱스 = ID)
		if (m_boneOffsets.Count() <= m_boneCounter) m_boneOffsets.Resize(m_boneCounter + 1);
		m_boneOffsets[m_boneCounter] = offset;
		if (m_boneHashByID.Count() <= m_boneCounter) m_boneHashByID.Resize(m_boneCounter + 1);
		m_boneHashByID[m_boneCounter] = nameHash;

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

		MGF_LOG_WARN("Vertex has too many bone influences. Limit is {}", MAX_BONE_INFLUENCE);
	}

	void Skeleton::SetData(const BoneMap& map, int32 count)
	{
		m_boneInfoMap = map;
		m_boneCounter = count;

		// 맵 데이터를 기반으로 벡터 캐시 재구축
		m_boneOffsets.Assign(count, glm::mat4(1.0f));
		m_boneHashByID.Assign(count, 0);

		m_parentIndices.Assign(count, -1);
		for (const auto& [hash, info] : m_boneInfoMap)
		{
			if (info.id >= 0 && info.id < count)
			{
				m_boneOffsets[info.id] = info.offset;
				m_boneHashByID[info.id] = hash;
			}
		}
	}

	const Skeleton::BoneMap& Skeleton::GetBoneInfoMap() const { return m_boneInfoMap; }

	int32 Skeleton::GetBoneCount() const { return m_boneCounter; }

	int32 Skeleton::GetBoneID(uint32 nameHash) const
	{
		auto infoPtr = m_boneInfoMap.Find(nameHash);
		if (infoPtr != nullptr) return infoPtr->id;
		return -1;
	}

	int32 Skeleton::GetBoneID(const SString& name) const
	{
		return GetBoneID(name.Hash());
	}

	const glm::mat4& Skeleton::GetBoneOffset(int32 boneID) const
	{
		if (CommonUtils::IsInRange<usize>(boneID, 0, m_boneOffsets.Count()))
			return m_boneOffsets[boneID];
		return GraphicsUtils::GetIdentityMatrix();
	}

	/*========================================//
	//   For GPU-Driven skeleton instancing   //
	//========================================*/
	uint32 Skeleton::GetBoneHash(int32 id) const
	{
		if (CommonUtils::IsInRange<usize>(id, 0, m_boneHashByID.Count()))
			return m_boneHashByID[id];
		return 0;
	}
}