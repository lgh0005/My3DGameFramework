#pragma once
#include "RawVertex.h"

struct RawMesh
{
	std::string name;
	uint32 materialIndex; // RawMaterial 리스트의 인덱스

	// 이 메쉬가 뼈대 애니메이션을 사용하는지 여부
	bool isSkinned = false;

	// 정점 데이터 저장소 (둘 중 하나만 채워짐)
	std::vector<RawStaticVertex>  staticVertices;
	std::vector<RawSkinnedVertex> skinnedVertices;

	// 인덱스 (인덱스는 공통으로 사용)
	std::vector<uint32> indices;

	// 바운딩 박스 (AABB) - 엔진의 절두체 컬링(Culling)을 위해 필수
	// 초기값은 뒤집어서 설정 (Min은 최대값으로, Max는 최소값으로)
	glm::vec3 aabbMin = glm::vec3(FLT_MAX); // FLT_MAX
	glm::vec3 aabbMax = glm::vec3(-FLT_MAX); // -FLT_MAX
};