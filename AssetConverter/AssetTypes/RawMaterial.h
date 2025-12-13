#pragma once
#include "RawTexture.h"

struct RawMaterial
{
	std::string name;

    // 1. PBR Factors (텍스처가 없을 때 사용될 기본값)
    // 텍스처가 있으면 이 값과 곱(Multiply)해서 쓰거나, 텍스처가 로딩되기 전까지 보여줄 색상으로 씁니다.
    glm::vec4 albedoFactor = { 1.0f, 1.0f, 1.0f, 1.0f }; // 기본 흰색
    glm::vec3 emissiveFactor = { 0.0f, 0.0f, 0.0f };     // 기본 빛 안 남
    float metallicFactor = 0.0f;                         // 기본 비금속 (플라스틱/나무 등)
    float roughnessFactor = 1.0f;                        // 기본 거침 (매끈하지 않음)

    // 2. 텍스처 목록
    // 고정된 변수 이름(diffusePath 등) 대신 유연한 리스트 사용
    std::vector<RawTexture> textures;

    // [편의 함수] 특정 타입의 텍스처 경로를 찾아서 반환 (파일 저장할 때 꿀 기능)
    std::string GetTexturePath(RawTextureType type) const;
};