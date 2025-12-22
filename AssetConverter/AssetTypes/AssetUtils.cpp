#include "pch.h"
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
    }
}

