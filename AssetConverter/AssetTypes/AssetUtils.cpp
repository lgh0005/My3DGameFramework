#include "pch.h"
#include "AssetUtils.h"

namespace MGF3D
{
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

    /*=============================//
    //  asset format util methods  //
    //=============================*/
    void AssetUtils::WriteRawMaterial(std::ofstream& file, const AssetFmt::RawMaterial& mat)
    {
        AssetUtils::WriteString(file, mat.name);
        AssetUtils::WriteData(file, mat.albedoFactor);
        AssetUtils::WriteData(file, mat.emissiveFactor);
        AssetUtils::WriteData(file, mat.emissiveStrength);
        AssetUtils::WriteData(file, mat.metallicFactor);
        AssetUtils::WriteData(file, mat.roughnessFactor);
        uint32 texCount = (uint32)mat.textures.size();
        AssetUtils::WriteData(file, texCount);
        for (const auto& tex : mat.textures)
        {
            AssetUtils::WriteString(file, tex.fileName);
            AssetUtils::WriteData(file, tex.type);
        }
    }

    void AssetUtils::WriteRawMesh(std::ofstream& file, const AssetFmt::RawMesh& mesh)
    {
        AssetUtils::WriteString(file, mesh.name);
        AssetUtils::WriteData(file, mesh.materialIndex);
        AssetUtils::WriteData(file, mesh.isSkinned);
        AssetUtils::WriteData(file, mesh.aabbMin);
        AssetUtils::WriteData(file, mesh.aabbMax);
        AssetUtils::WriteVector(file, mesh.staticVertices);
        AssetUtils::WriteVector(file, mesh.skinnedVertices);
        AssetUtils::WriteVector(file, mesh.indices);
    }

    void AssetUtils::WriteRawNodes(std::ofstream& file, const std::vector<AssetFmt::RawNode>& nodes)
    {
        // 1. 노드 개수 저장 (vector size)
        uint32 count = (uint32)nodes.size();
        WriteData(file, count);

        // 2. 각 노드 정보 순회 저장
        for (const auto& node : nodes)
        {
            // 이름 (가변 길이 문자열)
            WriteString(file, node.name);

            // 부모 인덱스 (int32)
            WriteData(file, node.parentIndex);

            // 로컬 변환 행렬 (glm::mat4)
            WriteData(file, node.localTransform);

            // 메쉬 인덱스 정보 저장
            WriteVector(file, node.meshIndices);

            // 자식 인덱스 저장
            WriteVector(file, node.children);
        }
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
            nodes[i].name = ReadString(file);
            nodes[i].parentIndex = ReadData<int32>(file);
            nodes[i].localTransform = ReadData<glm::mat4>(file);

            // TODO : 해당 부분들은 ReadVector로 대체할 수 있다면 대체 필요.

            // 메쉬 인덱스 정보 읽기
            uint32 meshCount = ReadData<uint32>(file);
            nodes[i].meshIndices.resize(meshCount);
            for (uint32 m = 0; m < meshCount; ++m)
                nodes[i].meshIndices[m] = ReadData<uint32>(file);

            // 자식 노드 인덱스 읽기
            uint32 childCount = ReadData<uint32>(file);
            nodes[i].children.resize(childCount);
            for (uint32 c = 0; c < childCount; ++c)
                nodes[i].children[c] = ReadData<int32>(file);
        }

        return nodes;
    }

    void AssetUtils::WriteKeyVector3(std::ofstream& file, const AssetFmt::RawKeyPosition& key)
    {
        WriteData(file, key.time);
        WriteData(file, key.vec3);
    }

    void AssetUtils::WriteKeyQuaternion(std::ofstream& file, const AssetFmt::RawKeyRotation& key)
    {
        WriteData(file, key.time);
        WriteData(file, key.quat);
    }

    void AssetUtils::WriteRawAnimation(std::ofstream& file, const AssetFmt::RawAnimation& anim)
    {
        // 1. Header
        WriteData(file, anim.magic);
        WriteData(file, anim.version);
        WriteString(file, anim.name);
        WriteData(file, anim.duration);
        WriteData(file, anim.ticksPerSecond);

        // 2. Baking Data
        WriteData(file, anim.frameRate);
        WriteData(file, anim.frameCount);
        WriteData(file, anim.boneCount);
        WriteVector(file, anim.bakedMatrices);

        // 2. Channels
        uint32 channelCount = (uint32)anim.channels.size();
        WriteData(file, channelCount);

        for (const auto& ch : anim.channels)
        {
            WriteString(file, ch.nodeName);

            // Positions
            uint32 pCount = (uint32)ch.positions.size();
            WriteData(file, pCount);
            for (const auto& key : ch.positions) WriteKeyVector3(file, key);

            // Rotations
            uint32 rCount = (uint32)ch.rotations.size();
            WriteData(file, rCount);
            for (const auto& key : ch.rotations) WriteKeyQuaternion(file, key);

            // Scales
            uint32 sCount = (uint32)ch.scales.size();
            WriteData(file, sCount);
            for (const auto& key : ch.scales) WriteKeyVector3(file, key);
        }
    }
}

