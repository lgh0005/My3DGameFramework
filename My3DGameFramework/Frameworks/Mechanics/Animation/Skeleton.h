#pragma once
namespace MGF3D
{
	MGF_CLASS_PTR(Skeleton)
		class Skeleton
	{
		MGF_DISABLE_COPY(Skeleton)

	public:
		~Skeleton();
		static SkeletonPtr Create
		(
			uint32 boneCount,
			Vector<mat4>&& offsets,
			Vector<int32>&& parentIndices,
			HashMap<StringHash, int32>&& nameToIdMap
		);

	public:
		uint32 GetBoneCount() const { return m_boneCount; }
		const Vector<mat4>& GetBoneOffsets() const { return m_boneOffsets; }
		const Vector<int32>& GetParentIndices() const { return m_parentIndices; }
		int32 GetBoneID(StringHash nameHash) const;

	private:
		Skeleton();
		void Init
		(
			uint32 boneCount,
			Vector<mat4>&& offsets,
			Vector<int32>&& parentIndices,
			HashMap<StringHash, int32>&& nameToIdMap
		);

		uint32 m_boneCount{ 0 };
		Vector<mat4> m_boneOffsets;
		Vector<int32> m_parentIndices;
		HashMap<StringHash, int32> m_boneMap;
	};
}