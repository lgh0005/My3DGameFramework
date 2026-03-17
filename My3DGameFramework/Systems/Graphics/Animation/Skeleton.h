#pragma once
#include "Utils/AssetFormat.h"
#include "Layouts/Vertex.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Skeleton)
	class Skeleton
	{
		MGF_DISABLE_COPY(Skeleton)

	public:
		using BoneMap = SMap<uint32, AssetFmt::RawBoneInfo>;

	public:
		Skeleton();
		~Skeleton();

	public:
		static void InitializeVertexBoneData(SkinnedVertex& vertex);
		static void AddBoneWeightToVertex(SkinnedVertex& vertex, int32 boneID, float weight);

		void SetData(const BoneMap& map, int32 count);
		const BoneMap& GetBoneInfoMap() const;
		int32 GetBoneCount() const;
		int32 GetBoneID(uint32 nameHash) const;
		int32 GetBoneID(const SString& name) const;
		int32 AddBone(const SString& name, const mat4& offset);
		const mat4& GetBoneOffset(int32 boneID) const;

	/*========================================//
	//   For GPU-Driven skeleton instancing   //
	//========================================*/
	public:
		void SetParentIndices(const SVector<int32>& parents) { m_parentIndices = parents; }
		const SVector<int32>& GetParentIndices() const { return m_parentIndices; }
		uint32 GetBoneHash(int32 id) const;

	private:
		BoneMap m_boneInfoMap;
		int32 m_boneCounter{ 0 };

		SVector<mat4> m_boneOffsets;
		SVector<int32> m_parentIndices; // GPU 연산용 부모 인덱스 (Index = BoneID, Value = ParentBoneID)
		SVector<uint32> m_boneHashByID; // ID로 이름을 찾기 위한 캐시 (Index = BoneID)
	};
}