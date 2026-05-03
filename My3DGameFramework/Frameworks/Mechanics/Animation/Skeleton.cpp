#include "FrameworkPch.h"
#include "Skeleton.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Skeleton::Skeleton() = default;
	Skeleton::~Skeleton() = default;

	SkeletonPtr Skeleton::Create
	(
		uint32 boneCount,
		Vector<mat4>&& offsets,
		Vector<int32>&& parentIndices,
		HashMap<StringHash, int32>&& nameToIdMap
	)
	{
		SkeletonPtr skeleton = SkeletonPtr(new Skeleton());
		skeleton->Init
		(
			boneCount,
			std::move(offsets),
			std::move(parentIndices),
			std::move(nameToIdMap)
		);
		return skeleton;
	}

	/*========================//
	//     Initialization     //
	//========================*/
	void Skeleton::Init
	(
		uint32 boneCount,
		Vector<mat4>&& offsets,
		Vector<int32>&& parentIndices,
		HashMap<StringHash, int32>&& nameToIdMap
	)
	{
		m_boneCount = boneCount;
		m_boneOffsets = std::move(offsets);
		m_parentIndices = std::move(parentIndices);
		m_boneMap = std::move(nameToIdMap);
	}

	/*========================//
	//      Data Getters      //
	//========================*/
	int32 Skeleton::GetBoneID(StringHash nameHash) const
	{
		auto it = m_boneMap.find(nameHash);
		if (it != m_boneMap.end()) return it->second;
		return -1;
	}
}