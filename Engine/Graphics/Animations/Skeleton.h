#pragma once
#include "Misc/AssetFormat.h"
#include "Graphics/Layouts/Vertex.h"

CLASS_PTR(Skeleton)
class Skeleton
{
public:
	~Skeleton();
	static SkeletonUPtr Create();
	static void InitializeVertexBoneData(SkinnedVertex& vertex);
	static void AddBoneWeightToVertex(SkinnedVertex& vertex, int32 boneID, float weight);
	using BoneMap = std::unordered_map<uint32, AssetFmt::RawBoneInfo>;

	void SetData(const BoneMap& map, int32 count);
	const BoneMap& GetBoneInfoMap() const;
	int32 GetBoneCount() const; 
	int32 GetBoneID(uint32 nameHash) const;
	int32 GetBoneID(const std::string& name) const;
	int32 AddBone(const std::string& name, const glm::mat4& offset);
	const glm::mat4& GetBoneOffset(int32 boneID) const;

/*========================================//
//   For GPU-Driven skeleton instancing   //
//========================================*/
public:
	void SetParentIndices(const std::vector<int32>& parents) { m_parentIndices = parents; }
	const std::vector<int32>& GetParentIndices() const { return m_parentIndices; }
	uint32 GetBoneHash(int32 id) const;

private:
	Skeleton();
	BoneMap m_boneInfoMap;
	int32 m_boneCounter	{ 0 };
	std::vector<glm::mat4> m_boneOffsets;

	std::vector<int32> m_parentIndices; // GPU 연산용 부모 인덱스 (Index = BoneID, Value = ParentBoneID)
	std::vector<uint32> m_boneHashByID; // ID로 이름을 찾기 위한 캐시 (Index = BoneID)
};