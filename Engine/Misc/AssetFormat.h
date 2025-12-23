#pragma once
#define MAX_BONE_INFLUENCE 4

namespace AssetFmt
{
    // RawTexture
    enum class RawTextureType : uint8
    {
        None = 0, Albedo, Normal, Emissive,
        ORM, AmbientOcclusion, Metallic,
        Roughness, Specular, Height,
        Glossiness, MAX
    };

    struct RawTexture
    {
        std::string fileName;
        AssetFmt::RawTextureType type;
    };

    // RawVertex
    struct RawStaticVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
    };

    struct RawSkinnedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
        int32     boneIDs[MAX_BONE_INFLUENCE];
        float     weights[MAX_BONE_INFLUENCE];

        RawSkinnedVertex();
    };

    struct KeyVector3 { glm::vec3 vec3; float time; };
    struct KeyQuaternion { glm::quat quat; float time; };

    using RawKeyPosition = KeyVector3;
    using RawKeyScale = KeyVector3;
    using RawKeyRotation = KeyQuaternion;

    // RawBone
    struct RawBoneInfo
    {
        uint32 id;
        glm::mat4 offset;
    };

    // RawImage
    struct RawImage
    {
        int32 width = 0;
        int32 height = 0;
        int32 channels = 0;
        std::vector<uint8> pixels;
        bool IsValid() const;
    };

    // RawMaterial
    struct RawMaterial
    {
        std::string name;
        glm::vec4 albedoFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec3 emissiveFactor = { 0.0f, 0.0f, 0.0f };
        float emissiveStrength = 0.0f;
        float metallicFactor = 0.0f;
        float roughnessFactor = 1.0f;
        std::vector<AssetFmt::RawTexture> textures;
        std::string GetTexturePath(AssetFmt::RawTextureType type) const;
    };

    // RawMesh
    struct RawMesh
    {
        std::string name;
        uint32 materialIndex;
        bool isSkinned = false;
        std::vector<AssetFmt::RawStaticVertex>  staticVertices;
        std::vector<AssetFmt::RawSkinnedVertex> skinnedVertices;
        std::vector<uint32> indices;
        glm::vec3 aabbMin = glm::vec3(FLT_MAX);
        glm::vec3 aabbMax = glm::vec3(-FLT_MAX);
    };

    // RawNode
    struct RawNode
    {
        std::string name;
        int32_t parentIndex = -1; // -1이면 루트
        glm::mat4 localTransform; // 초기 변환 (T-Pose)
    };

    // RawAnimChannel
    struct RawAnimChannel
    {
        std::string nodeName; // 움직일 노드의 이름 (RawNode의 name과 매칭)
        std::vector<RawKeyPosition> positions;
        std::vector<RawKeyRotation> rotations;
        std::vector<RawKeyScale>    scales;
    };

    // RawAnimation
    struct RawAnimation
    {
        uint32 magic = 0x414E494D; // 'ANIM'
        uint32 version = 1;
        std::string name;
        float duration;
        float ticksPerSecond;
        std::vector<RawAnimChannel> channels;
    };

    // RawModel
    struct RawModel
    {
        uint32 magicNumber = 0x4D594D44; // 'MYMD'
        uint32 version = 2;
        std::vector<AssetFmt::RawMaterial> materials;
        std::vector<AssetFmt::RawMesh> meshes;
        std::vector<AssetFmt::RawBoneInfo> boneOffsetInfos;
        bool hasSkeleton = false;
        std::vector<AssetFmt::RawNode> nodes;
        glm::vec3 globalAABBMin = glm::vec3(FLT_MAX);
        glm::vec3 globalAABBMax = glm::vec3(-FLT_MAX);
    };
}