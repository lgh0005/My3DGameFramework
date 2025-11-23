#pragma once
#include "Graphics/Vertex.h"

#pragma region FORWARD_DECLARATION
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType : int32;
#pragma endregion

#pragma region CONVERT_STRUCTS
struct TempMesh
{
    uint32_t materialIndex;
    std::vector<SkinnedVertex> vertices;
    std::vector<uint32_t> indices;
};

struct TempMaterial
{
    std::string diffuseMapPath;
    std::string specularMapPath;
    std::string emissionMapPath;
    std::string normalMapPath;
    std::string heightMapPath;
    // ... (나중에 shininess 등 PBR 데이터 추가) ...
};
#pragma endregion

class ModelConverter
{
public:
    static bool Convert(const std::string& inputPath, 
                        const std::string& outputPath);

private:
    ModelConverter() = default;
    bool RunConversion(const std::string& inputPath, const std::string& outputPath);

    // assimp 데이터 추출 메서드
    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    TempMaterial ProcessMaterial(aiMaterial* material);
    std::string GetTexturePath(aiMaterial* material, aiTextureType type);
    void ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, aiMesh* mesh);

    // 커스텀 바이너리 파일 작성 메서드
    bool WriteCustomModelFile(const std::string& outputPath);

    // 뼈 헬퍼 메서드
    void SetVertexBoneDataToDefault(SkinnedVertex& vertex);
    void SetVertexBoneData(SkinnedVertex& vertex, int boneID, float weight);

private:
    std::vector<TempMesh>     m_meshes;     // 추출된 메쉬 데이터
    std::vector<TempMaterial> m_materials;  // 추출된 머티리얼 데이터

    std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
    int32 m_boneCounter = 0;
    bool  m_hasSkeleton = false;

    // 텍스처 경로를 찾기 위한 원본 모델 파일의 디렉터리
    std::string m_modelDirectory;
};