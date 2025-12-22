#pragma once
#include "Resources/Resource.h"
#include "Misc/AssetFormat.h" // 공용 포맷
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SceneModel)
CLASS_PTR(GameObject)
#pragma endregion

class SceneModel : public Resource
{
public:
    static const ResourceType s_ResourceType = ResourceType::Model; // 혹은 SceneModel로 타입 분리 가능
    virtual ResourceType GetResourceType() const override { return ResourceType::Model; }

    static SceneModelUPtr Load(const std::string& filename);

    // [핵심 기능] 이 모델의 계층 구조를 바탕으로 실제 GameObject 트리를 생성합니다.
    GameObjectUPtr CreateGameObject();

public:
    // Getter
    const std::vector<MeshPtr>& GetMeshes() const { return m_meshes; }
    const std::vector<MaterialPtr>& GetMaterials() const { return m_materials; }
    const std::vector<AssetFmt::RawNode>& GetNodes() const { return m_nodes; }

private:
    SceneModel() = default;

    bool LoadByBinary(const std::string& filename);
    bool LoadByAssimp(const std::string& filename);

    // 내부 헬퍼
    TexturePtr LoadTexture(const std::string& path, const std::filesystem::path& parentDir);
    GameObjectUPtr CreateGameObjectRecursive(const AssetFmt::RawNode& node);

    // Assimp 로드 헬퍼 (V3 로직 이식)
    int32 ProcessNodeAssimp(aiNode* node, const aiScene* scene, int32 parentIndex,
        std::unordered_map<std::string, int32>& boneMap, int32& boneCounter);
    void ProcessMeshAssimp(aiMesh* mesh, const aiScene* scene,
        std::unordered_map<std::string, int32>& boneMap, int32& boneCounter);

private:
    // 파일에서 읽은 원본 데이터 (AssetFmt 구조 유지)
    std::vector<AssetFmt::RawNode>     m_nodes;
    std::vector<AssetFmt::RawBoneInfo> m_boneInfos;

    // 엔진 리소스 (실제 렌더링에 사용)
    std::vector<MeshPtr>     m_meshes;
    std::vector<MaterialPtr> m_materials;
};