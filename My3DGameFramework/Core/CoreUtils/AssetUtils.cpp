#include "CorePch.h"
#include "AssetUtils.h"

namespace MGF3D
{
    /*========================================//
    //   ModelConverter의 파일 쓰기 유틸 함수   //
    //========================================*/
    void AssetUtils::WriteString(OutputFileStream& file, const String& str)
    {
        uint32 len = (uint32)str.length();
        WriteData(file, len);
        file.write(str.c_str(), len);
    }

    /*========================================//
    //   ModelConverter의 파일 읽기 유틸 함수   //
    //========================================*/
    String AssetUtils::ReadString(InputFileStream& file)
    {
        uint32 len = 0;
        file.read(reinterpret_cast<char*>(&len), sizeof(uint32));
        if (len == 0) return "";

        String str(len, '\0');
        file.read(&str[0], len);
        return str;
    }

    /*==================================//
    //  asset format read util methods  //
    //==================================*/
    RawMaterial AssetUtils::ReadRawMaterial(InputFileStream& file)
    {
        RawMaterial mat;

        // 1. 이름
        mat.name = AssetUtils::ReadString(file);

        // 2. Factors
        mat.albedoFactor = AssetUtils::ReadData<vec4>(file);
        mat.emissiveFactor = AssetUtils::ReadData<vec3>(file);
        mat.emissiveStrength = AssetUtils::ReadData<float>(file);
        mat.metallicFactor = AssetUtils::ReadData<float>(file);
        mat.roughnessFactor = AssetUtils::ReadData<float>(file);

        // 3. Textures (Vector of structs)
        uint32_t texCount = AssetUtils::ReadData<uint32>(file);
        mat.textures.resize(texCount);
        for (uint32_t i = 0; i < texCount; ++i)
        {
            mat.textures[i].fileName = AssetUtils::ReadString(file);
            mat.textures[i].type = AssetUtils::ReadData<RawTextureType>(file);
        }

        return mat;
    }

    RawMesh AssetUtils::ReadRawMesh(InputFileStream& file)
    {
        RawMesh mesh;

        // 1. 기본 정보
        mesh.name = AssetUtils::ReadString(file);
        mesh.materialIndex = AssetUtils::ReadData<uint32_t>(file);
        mesh.isSkinned = AssetUtils::ReadData<bool>(file);

        // 2. AABB
        mesh.aabbMin = AssetUtils::ReadData<vec3>(file);
        mesh.aabbMax = AssetUtils::ReadData<vec3>(file);

        // 3. Vectors (AssetUtils::WriteVector에 대응)
        AssetUtils::ReadVector(file, mesh.staticVertices);
        AssetUtils::ReadVector(file, mesh.skinnedVertices);
        AssetUtils::ReadVector(file, mesh.indices);

        return mesh;
    }

    Vector<RawNode> AssetUtils::ReadRawNodes(InputFileStream& file)
    {
        Vector<RawNode> nodes;

        // 1. 노드 개수 읽기
        uint32 count = ReadData<uint32>(file);
        nodes.resize(count);

        // 2. 각 노드 정보 읽기
        for (uint32 i = 0; i < count; ++i)
        {
            auto& node = nodes[i];
            node.name = ReadString(file);
            node.parentIndex = ReadData<int32>(file);
            node.localTransform = ReadData<mat4>(file);
            AssetUtils::ReadVector(file, node.meshIndices);
            AssetUtils::ReadVector(file, node.children);
        }

        return nodes;
    }

    RawKeyPosition AssetUtils::ReadKeyPosition(InputFileStream& file)
    {
        RawKeyPosition key;
        key.time = ReadData<float>(file);
        key.position = ReadData<vec3>(file);
        return key;
    }

    RawKeyRotation AssetUtils::ReadKeyRotation(InputFileStream& file)
    {
        RawKeyRotation key;
        key.time = ReadData<float>(file);
        key.rotation = ReadData<quat>(file);
        return key;
    }

    RawKeyScale AssetUtils::ReadKeyScale(InputFileStream& file)
    {
        RawKeyScale key;
        key.time = ReadData<float>(file);
        key.scale = ReadData<vec3>(file);
        return key;
    }

    RawAnimation AssetUtils::ReadRawAnimation(InputFileStream& file)
    {
        RawAnimation anim;

        // 1. Header
        anim.magic = ReadData<uint32>(file);
        anim.version = ReadData<uint32>(file);
        anim.name = ReadString(file);
        anim.duration = ReadData<float>(file);
        anim.ticksPerSecond = ReadData<float>(file);

        // 2. Baking Data
        ReadData<float>(file, anim.frameRate);
        ReadData<uint32>(file, anim.frameCount);
        ReadData<uint32>(file, anim.boneCount);
        ReadVector<mat4>(file, anim.bakedMatrices);

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
                ch.positions.push_back(ReadKeyPosition(file));

            // Rotations
            uint32 rCount = ReadData<uint32>(file);
            ch.rotations.reserve(rCount);
            for (uint32 k = 0; k < rCount; ++k)
                ch.rotations.push_back(ReadKeyRotation(file));

            // Scales
            uint32 sCount = ReadData<uint32>(file);
            ch.scales.reserve(sCount);
            for (uint32 k = 0; k < sCount; ++k)
                ch.scales.push_back(ReadKeyScale(file));
        }

        return anim;
    }
}


