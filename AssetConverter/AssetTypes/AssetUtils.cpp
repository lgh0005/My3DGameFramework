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

