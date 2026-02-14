#include "EnginePch.h"
#include "Model.h"
#include "Scene/Scene.h"
#include "Object/GameObject.h"
#include "Components/Transform.h"
#include "Components/Animator.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Image.h"
#include "Resources/Textures/TextureUtils.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Animations/Skeleton.h"
#include "Graphics/Layouts/VertexLayout.h"
#include "Resources/Programs/Program.h"
#include "Misc/AssetFormat.h"
#include "Misc/AssetUtils.h"
#include "Misc/Utils.h"

DECLARE_DEFAULTS_IMPL(Model)

SkinnedMeshPtr Model::GetSkinnedMesh(int index) const
{
    return std::static_pointer_cast<SkinnedMesh>(m_meshes[index]);
}

StaticMeshPtr Model::GetStaticMesh(int index) const
{
    return std::static_pointer_cast<StaticMesh>(m_meshes[index]);
}

ModelPtr Model::Load(const ModelDesc& desc)
{
    // 1. 객체 생성 (기본 생성자 호출)
    auto model = ModelPtr(new Model());
    model->m_desc = desc;

    // 3. 확장자 체크 (m_desc.path 기반)
    std::string ext = fs::path(model->m_desc.path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".mymodel")
    {
        LOG_ERROR("Model::Load - Invalid extension: {}", model->m_desc.path);
        return nullptr;
    }

    // 4. 바이너리 로드 시작
    if (!model->LoadByBinary()) return nullptr;

    return model;
}

/*======================================//
//   3d model load instancing methods   //
//======================================*/
GameObjectUPtr Model::Instantiate(Scene* scene, Animator* animator)
{
    if (!scene || m_nodes.empty()) return nullptr;

    // 1. 생성된 GameObject들을 인덱스로 접근하기 위한 캐시
    std::vector<GameObject*> createdGOs(m_nodes.size(), nullptr);
    GameObjectUPtr rootUPtr = nullptr;

    // 2. m_nodes는 부모->자식 순서(DFS 순서)로 저장되어 있음이 보장됩니다.
    for (usize i = 0; i < m_nodes.size(); ++i)
    {
        const auto& node = m_nodes[i];

        // 2-1. 부모 찾기 (RawNode에 저장된 parentIndex 활용)
        GameObject* parentGO = nullptr;
        if (node.parentIndex >= 0 && node.parentIndex < (int32)createdGOs.size())
            parentGO = createdGOs[node.parentIndex];

        // 2-2. 단일 노드 생성 (재귀 호출 없음)
        auto goUPtr = CreateGameObjectForSingleNode(scene, parentGO, node, animator);

        // 3. 캐싱
        createdGOs[i] = goUPtr.get();

        // 4. 씬 등록 및 루트 반환 처리
        if (i == 0) rootUPtr = std::move(goUPtr);
        else scene->AddGameObject(std::move(goUPtr));
    }

    return std::move(rootUPtr);
}

// TODO : 이 메서드 조금 정리할 필요가 있음
GameObjectUPtr Model::CreateGameObjectForSingleNode
(
    Scene* scene,
    GameObject* parent,
    const AssetFmt::RawNode& node,
    Animator* animator
)
{
    // 1. 현재 노드(컨테이너) GameObject 생성
    auto goUPtr = GameObject::Create();
    GameObject* currentGO = goUPtr.get();

    // 2. 기본 정보 설정
    currentGO->SetName(node.name);
    currentGO->GetTransform().SetLocalMatrix(node.localTransform);

    // 3. 계층 구조 연결
    if (parent) currentGO->SetParent(parent);

    // 4. Mesh Renderer 부착
    // 메쉬가 여러 개면 자식 오브젝트를 만들어서 각각 붙여줍니다.
    usize meshCount = node.meshIndices.size();
    bool useSubObject = (meshCount > 1); // 2개 이상일 때만 자식 분리

    for (usize i = 0; i < meshCount; ++i)
    {
        uint32 meshIndex = node.meshIndices[i];
        if (meshIndex >= m_meshes.size()) continue;

        auto meshRes = m_meshes[meshIndex];

        // 머티리얼 찾기
        uint32 matIdx = 0;
        if (meshIndex < m_meshMaterialIndices.size())
            matIdx = m_meshMaterialIndices[meshIndex];

        MaterialPtr material = nullptr;
        if (matIdx < m_materials.size()) material = m_materials[matIdx];

        // 1. 렌더러를 붙일 대상(Target) 결정 및 임시 저장
        GameObject* targetGO = currentGO;
        GameObjectUPtr subGOPtr = nullptr; // 자식 오브젝트를 잠시 보관할 변수

        if (useSubObject)
        {
            // 자식 오브젝트 생성
            auto subGO = GameObject::Create();
            subGO->SetName(node.name + "_Sub_" + std::to_string(i));
            subGO->SetParent(currentGO);

            // 타겟을 자식으로 변경
            targetGO = subGO.get();

            // 씬에 바로 넣지 말고, 일단 지역 변수에 홀딩해둡니다.
            subGOPtr = std::move(subGO);
        }

        // 2. 컴포넌트를 먼저 다 붙입니다!
        
        // 2-1. 렌더러 부착
        switch (meshRes->GetResourceType())
        {
            case ResourceType::StaticMesh:
            {
                auto staticMesh = std::static_pointer_cast<StaticMesh>(meshRes);
                auto renderer = StaticMeshRenderer::Create(staticMesh, material);
                targetGO->AddComponent(std::move(renderer));
                break;
            }
            case ResourceType::SkinnedMesh:
            {
                auto skinnedMesh = std::static_pointer_cast<SkinnedMesh>(meshRes);
                auto renderer = SkinnedMeshRenderer::Create(skinnedMesh, material, animator);
                targetGO->AddComponent(std::move(renderer));
                break;
            }
        }

        // 2-2. [TEMP] 아웃라인 부착
        {
            auto outline = MeshOutline::Create(glm::vec3(1.0f, 1.0f, 0.0f), 3.0f);
            targetGO->AddComponent(std::move(outline));
        }

        // 컴포넌트 부착이 끝난 후, 씬에 등록합니다.
        if (useSubObject && subGOPtr)
            scene->AddGameObject(std::move(subGOPtr));
    }

    return goUPtr;
}

/*========================================================//
//   .mymodel file load process methods : .mymodel file   //
//========================================================*/
bool Model::LoadByBinary()
{
    std::string actualPath = RESOURCE.ResolvePath(m_desc.path);
    std::ifstream inFile(actualPath, std::ios::binary);
    if (!inFile)
    {
        LOG_ERROR("Failed to open binary model file (V3): {}", actualPath);
        return false;
    }

    std::filesystem::path modelDir = std::filesystem::path(actualPath).parent_path();

    // 데이터를 받아올 지역 변수 (참조로 넘겨서 채움)
    uint32 matCount = 0;
    uint32 meshCount = 0;
    bool hasSkeleton = false;
    if (!ReadBinaryModelHeader(inFile, matCount, meshCount, hasSkeleton)) 
        return false;

    // 2. Nodes Hierarchy Read
    ReadBinaryNodes(inFile);

    // 3. Skeleton Read
    if (hasSkeleton)
    {
        m_skeleton = Skeleton::Create();
        ReadBinarySkeleton(inFile);
    }
    else m_skeleton = nullptr;

    // 4. Materials Read
    ReadBinaryMaterials(inFile, matCount, modelDir);

    // 5. Meshes Read
    ReadBinaryMeshes(inFile, meshCount);

    // 6. Skeletal Hierarchy Index Write
    LinkSkeletonHierarchy();

    LOG_INFO("Model loaded successfully (Binary): {}", actualPath);
    inFile.close();
    return true;
}

bool Model::ReadBinaryModelHeader(std::ifstream& inFile, uint32& outMatCount, uint32& outMeshCount, bool& outHasSkeleton)
{
    auto magic = AssetUtils::ReadData<uint32>(inFile);
    if (magic != 0x4D594D44)
    {
        LOG_ERROR("Invalid Magic Number");
        return false;
    }

    auto version = AssetUtils::ReadData<uint32>(inFile);
    if (version != 3)
    {
        LOG_WARN("Version mismatch! Expected 2, Got {}", version);
    }

    // 값들을 읽어서 참조 인자에 할당
    outMatCount = AssetUtils::ReadData<uint32>(inFile);
    outMeshCount = AssetUtils::ReadData<uint32>(inFile);
    outHasSkeleton = AssetUtils::ReadData<bool>(inFile);

    // 전역 AABB는 현재 Model 클래스 멤버가 아니면 여기서 읽고 버리거나, 멤버에 저장합니다.
    auto globalMin = AssetUtils::ReadData<glm::vec3>(inFile);
    auto globalMax = AssetUtils::ReadData<glm::vec3>(inFile);

    return true;
}

void Model::ReadBinaryNodes(std::ifstream& inFile) 
{
    m_nodes = AssetUtils::ReadRawNodes(inFile); 
}

void Model::ReadBinarySkeleton(std::ifstream& inFile)
{
    // 로컬 변수에 먼저 읽어들임
    Skeleton::BoneMap boneMap;

    // 1. 뼈 정보 벡터 읽기 (Loop Read 필수!)
    uint32 bCount = AssetUtils::ReadData<uint32>(inFile);
    std::vector<AssetFmt::RawBoneInfo> rawBoneInfos(bCount);
    for (uint32 i = 0; i < bCount; ++i)
    {
        rawBoneInfos[i].id = AssetUtils::ReadData<uint32>(inFile);
        rawBoneInfos[i].offset = AssetUtils::ReadData<glm::mat4>(inFile);
    }

    // 2. 이름 매핑 읽기
    uint32 mapCount = AssetUtils::ReadData<uint32>(inFile);
    for (uint32 i = 0; i < mapCount; ++i)
    {
        std::string name = AssetUtils::ReadString(inFile);
        int32 id = AssetUtils::ReadData<int32>(inFile);

        uint32 nameHash = Utils::StrHash(name);
        AssetFmt::RawBoneInfo info;
        info.id = id;

        if (id >= 0 && id < (int32)rawBoneInfos.size())
            info.offset = rawBoneInfos[id].offset;

        boneMap[nameHash] = info;
    }

    // 3. 다 읽은 데이터를 Skeleton에 주입
    if (m_skeleton) m_skeleton->SetData(boneMap, (int32)bCount);
}

void Model::ReadBinaryMaterials(std::ifstream& inFile, uint32 matCount, const std::filesystem::path& modelDir)
{
    m_materials.resize(matCount);
    for (uint32 i = 0; i < matCount; ++i)
    {
        // Raw Data 읽기
        auto rawMat = AssetUtils::ReadRawMaterial(inFile);

        // Engine Material 생성
        auto material = Material::Create();
        material->albedoFactor = rawMat.albedoFactor;
        material->metallicFactor = rawMat.metallicFactor;
        material->roughnessFactor = rawMat.roughnessFactor;
        material->emissiveFactor = rawMat.emissiveFactor;
        material->emissionStrength = rawMat.emissiveStrength;

        TexturePtr glossinessTex = nullptr;
        for (const auto& rawTex : rawMat.textures)
        {
            if (rawTex.fileName.empty()) continue;

            // 1. 파일 이름에서 확장자를 뗀 'Key' 추출
            std::string texKey = fs::path(rawTex.fileName).stem().string();

            // 2. 리소스 매니저에게 이름으로 요청.
            auto texPtr = RESOURCE.Get<Texture>(texKey);
            if (!texPtr)
            {
                LOG_WARN("Model: Texture Key '{}' not found in Registry. Using default.", texKey);
                continue;
            }

            switch (rawTex.type)
            {
            case AssetFmt::RawTextureType::Albedo:            material->diffuse = texPtr; break;
            case AssetFmt::RawTextureType::Normal:            material->normal = texPtr; break;
            case AssetFmt::RawTextureType::ORM:               material->orm = texPtr; break;
            case AssetFmt::RawTextureType::Emissive:          material->emission = texPtr; break;
            case AssetFmt::RawTextureType::AmbientOcclusion:  material->ao = texPtr; break;
            case AssetFmt::RawTextureType::Roughness:         material->roughness = texPtr; break;
            case AssetFmt::RawTextureType::Metallic:          material->metallic = texPtr; break;
            case AssetFmt::RawTextureType::Specular:          material->specular = texPtr; break;
            case AssetFmt::RawTextureType::Height:            material->height = texPtr; break;
            case AssetFmt::RawTextureType::Glossiness:        glossinessTex = texPtr; break;
            }
        }

        // [Glossiness Fallback]
        if (!material->roughness && glossinessTex)
        {
            material->roughness = glossinessTex;
            material->useGlossinessAsRoughness = true;
        }

        m_materials[i] = std::move(material);
    }
}

void Model::ReadBinaryMeshes(std::ifstream& inFile, uint32 meshCount)
{
    m_meshes.clear();
    m_meshes.reserve(meshCount);
    m_meshMaterialIndices.clear();
    m_meshMaterialIndices.reserve(meshCount);

    for (uint32 i = 0; i < meshCount; ++i)
    {
        // 1. 파일에서 Raw 데이터 덩어리 읽기
        auto rawMesh = AssetUtils::ReadRawMesh(inFile);

        // 2. 타입에 따라 적절한 함수 호출 (분기 처리)
        if (rawMesh.isSkinned) CreateBinarySkinnedMesh(rawMesh);
        else CreateBinaryStaticMesh(rawMesh);
    }
}

void Model::CreateBinarySkinnedMesh(const AssetFmt::RawMesh& rawMesh)
{
    std::vector<SkinnedVertex> engineVerts(rawMesh.skinnedVertices.size());
    for (usize v = 0; v < rawMesh.skinnedVertices.size(); ++v)
    {
        const auto& src = rawMesh.skinnedVertices[v];
        auto& dst = engineVerts[v];

        dst.position = src.position;
        dst.normal = src.normal;
        dst.texCoord = src.texCoord;
        dst.tangent = src.tangent;

        for (int j = 0; j < 4; ++j) 
        {
            dst.weights[j] = src.weights[j];
            dst.boneIDs[j] = src.boneIDs[j];
        }
    }

    // 2. 엔진 메쉬 객체 생성
    auto mesh = SkinnedMesh::Create(engineVerts, rawMesh.indices, GL_TRIANGLES);

    // 3. 바운딩 박스 설정
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(rawMesh.aabbMin, rawMesh.aabbMax));
    
    // 4. 머티리얼 인덱스 저장
    m_meshMaterialIndices.push_back(rawMesh.materialIndex);

    // 5. 등록
    m_meshes.push_back(std::move(mesh));
}

void Model::CreateBinaryStaticMesh(const AssetFmt::RawMesh& rawMesh)
{
    // 1. 정점 변환 (Raw -> Engine)
    std::vector<StaticVertex> engineVerts(rawMesh.staticVertices.size());
    for (usize v = 0; v < rawMesh.staticVertices.size(); ++v)
    {
        const auto& src = rawMesh.staticVertices[v];
        auto& dst = engineVerts[v];

        dst.position = src.position;
        dst.normal = src.normal;
        dst.texCoord = src.texCoord;
        dst.tangent = src.tangent;
    }

    // 2. 엔진 메쉬 객체 생성
    auto mesh = StaticMesh::Create(engineVerts, rawMesh.indices, GL_TRIANGLES);

    // 3. 바운딩 박스 설정
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(rawMesh.aabbMin, rawMesh.aabbMax));

    // 4. 머티리얼 인덱스 저장
    m_meshMaterialIndices.push_back(rawMesh.materialIndex);

    // 5. 등록
    m_meshes.push_back(std::move(mesh));
}

/*============================//
//   skeleton helper method   //
//============================*/
void Model::LinkSkeletonHierarchy()
{
    if (!m_skeleton || m_nodes.empty()) return;

    int32 boneCount = m_skeleton->GetBoneCount();
    std::vector<int32> parentIndices(boneCount, -1);

    // 검색 속도를 높이기 위해 [NodeHash -> NodeIndex] 맵을 임시로 생성
    std::unordered_map<uint32, int32> nodeHashMap;
    nodeHashMap.reserve(m_nodes.size());
    for (int32 i = 0; i < m_nodes.size(); ++i)
        nodeHashMap[Utils::StrHash(m_nodes[i].name)] = i;

    // 모든 뼈를 순회하며 부모 찾기
    for (int32 i = 0; i < boneCount; ++i)
    {
        // 1. 현재 뼈의 해시로 노드 인덱스 찾기
        uint32 myHash = m_skeleton->GetBoneHash(i);
        auto itNode = nodeHashMap.find(myHash);
        if (itNode == nodeHashMap.end()) continue;

        int32 myNodeIdx = itNode->second;
        const auto& myNode = m_nodes[myNodeIdx];

        // 2. 해당 노드의 부모 확인
        if (myNode.parentIndex != -1)
        {
            const auto& parentNode = m_nodes[myNode.parentIndex];
            uint32 parentHash = Utils::StrHash(parentNode.name);

            // 4. 부모 노드가 '뼈' 목록에 있는지 확인
            int32 parentBoneID = m_skeleton->GetBoneID(parentHash);

            // 5. 있으면 그 ID를 부모 인덱스로 등록
            if (parentBoneID != -1)
                parentIndices[i] = parentBoneID;
        }
    }

    // 완성된 족보를 스켈레톤에 주입
    m_skeleton->SetParentIndices(parentIndices);
    LOG_INFO("Skeleton Hierarchy Linked: {} bones", boneCount);
}
