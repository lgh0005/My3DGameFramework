#pragma once

enum class RawTextureType : int8
{
    None = 0,
    Albedo,     // Diffuse Color
    Normal,     // Normal Map
    
    Emissive,   // Emission

    // [최종] 패킹된 텍스처 (Occlusion + Roughness + Metallic)
    ORM,        // Occlusion(R) + Roughness(G) + Metallic(B)

    // [입력] Assimp에서 읽어올 때 필요한 개별 채널 (나중에 ORM으로 합쳐짐)
    AmbientOcclusion,
    Metallic,
    Roughness,

    // [레거시 / 기타]
    Specular,
    Height,     // Displacement
    Shininess,  // [New] Glossiness / Specular Power

    MAX
};

struct RawTexture
{
    // 한 폴더 안에 모델과 텍스쳐를 같이 둔 채로 읽는 것을 가정
    std::string relativePath; // .asset 파일 기준 상대 경로 (예: "Textures/Sword_ORM.png")
    RawTextureType type;
};