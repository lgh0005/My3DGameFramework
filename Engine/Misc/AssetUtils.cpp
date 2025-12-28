#include "EnginePch.h"
#include "AssetUtils.h"

/*========================================//
//   ModelConverter의 파일 쓰기 유틸 함수   //
//========================================*/
void AssetUtils::WriteString(std::ofstream& file, const std::string& str)
{
    uint32 len = (uint32)str.length();
    WriteData(file, len);
    file.write(str.c_str(), len);
}

/*========================================//
//   ModelConverter의 파일 읽기 유틸 함수   //
//========================================*/
std::string AssetUtils::ReadString(std::ifstream& file)
{
    uint32 len = 0;
    file.read(reinterpret_cast<char*>(&len), sizeof(uint32));
    if (len == 0) return "";

    std::string str(len, '\0');
    file.read(&str[0], len);
    return str;
}

/*==================================//
//  asset format read util methods  //
//==================================*/
AssetFmt::RawMaterial AssetUtils::ReadRawMaterial(std::ifstream& file)
{
    AssetFmt::RawMaterial mat;

    // 1. 이름
    mat.name = AssetUtils::ReadString(file);

    // 2. Factors
    mat.albedoFactor = AssetUtils::ReadData<glm::vec4>(file);
    mat.emissiveFactor = AssetUtils::ReadData<glm::vec3>(file);
    mat.emissiveStrength = AssetUtils::ReadData<float>(file);
    mat.metallicFactor = AssetUtils::ReadData<float>(file);
    mat.roughnessFactor = AssetUtils::ReadData<float>(file);

    // 3. Textures (Vector of structs)
    uint32_t texCount = AssetUtils::ReadData<uint32_t>(file);
    mat.textures.resize(texCount);
    for (uint32_t i = 0; i < texCount; ++i)
    {
        mat.textures[i].fileName = AssetUtils::ReadString(file);
        mat.textures[i].type = AssetUtils::ReadData<AssetFmt::RawTextureType>(file);
    }

    return mat;
}

AssetFmt::RawMesh AssetUtils::ReadRawMesh(std::ifstream& file)
{
    AssetFmt::RawMesh mesh;

    // 1. 기본 정보
    mesh.name = AssetUtils::ReadString(file);
    mesh.materialIndex = AssetUtils::ReadData<uint32_t>(file);
    mesh.isSkinned = AssetUtils::ReadData<bool>(file);

    // 2. AABB
    mesh.aabbMin = AssetUtils::ReadData<glm::vec3>(file);
    mesh.aabbMax = AssetUtils::ReadData<glm::vec3>(file);

    // 3. Vectors (AssetUtils::WriteVector에 대응)
    AssetUtils::ReadVector(file, mesh.staticVertices);
    AssetUtils::ReadVector(file, mesh.skinnedVertices);
    AssetUtils::ReadVector(file, mesh.indices);

    return mesh;
}

std::vector<AssetFmt::RawNode> AssetUtils::ReadRawNodes(std::ifstream& file)
{
    std::vector<AssetFmt::RawNode> nodes;

    // 1. 노드 개수 읽기
    uint32 count = ReadData<uint32>(file);
    nodes.resize(count);

    // 2. 각 노드 정보 읽기
    for (uint32 i = 0; i < count; ++i)
    {
        auto& node = nodes[i];
        node.name = ReadString(file);
        node.parentIndex = ReadData<int32>(file);
        node.localTransform = ReadData<glm::mat4>(file);
        AssetUtils::ReadVector(file, node.meshIndices);
        AssetUtils::ReadVector(file, node.children);
    }

    return nodes;
}

AssetFmt::RawKeyPosition AssetUtils::ReadKeyVector3(std::ifstream& file)
{
    AssetFmt::RawKeyPosition key;
    key.time = ReadData<float>(file);
    key.vec3 = ReadData<glm::vec3>(file);
    return key;
}

AssetFmt::RawKeyRotation AssetUtils::ReadKeyQuaternion(std::ifstream& file)
{
    AssetFmt::RawKeyRotation key;
    key.time = ReadData<float>(file);
    key.quat = ReadData<glm::quat>(file);
    return key;
}

AssetFmt::RawAnimation AssetUtils::ReadRawAnimation(std::ifstream& file)
{
    AssetFmt::RawAnimation anim;

    // 1. Header
    anim.magic = ReadData<uint32>(file);
    anim.version = ReadData<uint32>(file);
    anim.name = ReadString(file);
    anim.duration = ReadData<float>(file);
    anim.ticksPerSecond = ReadData<float>(file);

    // 2. Channels
    uint32 channelCount = ReadData<uint32>(file);
    anim.channels.resize(channelCount);

    for (uint32 i = 0; i < channelCount; ++i)
    {
        auto& ch = anim.channels[i];

        // Node Name
        ch.nodeName = ReadString(file);

        // Positions
        uint32 pCount = ReadData<uint32>(file);
        ch.positions.reserve(pCount);
        for (uint32 k = 0; k < pCount; ++k)
            ch.positions.push_back(ReadKeyVector3(file));

        // Rotations
        uint32 rCount = ReadData<uint32>(file);
        ch.rotations.reserve(rCount);
        for (uint32 k = 0; k < rCount; ++k)
            ch.rotations.push_back(ReadKeyQuaternion(file));

        // Scales
        uint32 sCount = ReadData<uint32>(file);
        ch.scales.reserve(sCount);
        for (uint32 k = 0; k < sCount; ++k)
            ch.scales.push_back(ReadKeyVector3(file));
    }

    return anim;
}


