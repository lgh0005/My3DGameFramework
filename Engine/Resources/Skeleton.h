#pragma once
#include "Misc/AssetFormat.h"
#include "Graphics/Vertex.h"

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

private:
	Skeleton();
	BoneMap m_boneInfoMap;
	int32 m_boneCounter	{ 0 };
	std::vector<glm::mat4> m_boneOffsets;
};