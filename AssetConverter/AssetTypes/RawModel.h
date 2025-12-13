#pragma once
#include "RawMaterial.h"
#include "RawMesh.h"
#include "RawBone.h"

struct RawModel
{
	// 1. 파일 헤더 / 메타 데이터
	uint32 magicNumber = 0x4D594D44;
	uint32 version = 2;

	// 2. 핵심 데이터 블록
	std::vector<RawMaterial> materials;
	std::vector<RawMesh> meshes;

	// 3. 스켈레톤 (애니메이션) 데이터
	// [중요] 렌더링용: 오프셋 행렬 정보 (스키닝 계산에 필수)
	// 엔진의 m_boneInfoMap에 대응됩니다.
	std::vector<RawBoneInfo> boneOffsetInfos;
	bool hasSkeleton = false;

	// [중요] 애니메이션용: 계층 구조 및 키프레임 정보
	// 엔진이 나중에 애니메이션을 재생하려면 이 계층 구조가 필요합니다.
	std::vector<RawBone> flatSkeleton;

	// 4. 모델 전체의 바운딩 박스 (모든 메쉬의 AABB를 합친 것)
	glm::vec3 globalAABBMin = glm::vec3(FLT_MAX);
	glm::vec3 globalAABBMax = glm::vec3(-FLT_MAX);
};