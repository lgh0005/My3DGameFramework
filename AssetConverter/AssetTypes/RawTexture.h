#pragma once

enum class RawTextureType : uint8
{
    None = 0,
    Albedo,
    Normal,
    Emissive, 
    ORM,
    AmbientOcclusion,
    Metallic,
    Roughness,
    Specular,
    Height,
    Glossiness,
    MAX
};

struct RawTexture
{
    // 한 폴더 안에 모델과 텍스쳐를 같이 둔 채로 읽는 것을 가정
    //  // 규격: mymodel 파일과 같은 폴더에 있어야 함. (파일명만 저장)
    std::string fileName; // 예: "Sword_ORM.png"
    RawTextureType type;
};