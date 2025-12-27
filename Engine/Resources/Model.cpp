#include "EnginePch.h"
#include "Model.h"
#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Image.h"
#include "Resources/Texture.h"
#include "Resources/Skeleton.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Program.h"
#include "Misc/AssetFormat.h"
#include "Misc/AssetUtils.h"

SkinnedMeshPtr Model::GetSkinnedMesh(int index) const
{
    return std::static_pointer_cast<SkinnedMesh>(m_meshes[index]);
}

StaticMeshPtr Model::GetStaticMesh(int index) const
{
    return std::static_pointer_cast<StaticMesh>(m_meshes[index]);
}

ModelUPtr Model::Load(const std::string& filename)
{
    auto model = ModelUPtr(new Model());

    // 파일 확장명 비교 후 로드
    std::string ext = std::filesystem::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".mymodel")
    {
         if (!model->LoadByBinary(filename)) return nullptr;
    }
    else
    {
        if (!model->LoadByAssimp(filename)) return nullptr;
    }

    return std::move(model);
}

void Model::Draw(const Program* program) const
{
    for (auto& mesh : m_meshes)
        mesh->Draw(program);
}

/*======================================//
//   3d model load instancing methods   //
//======================================*/
GameObject* Model::Instantiate(Scene* scene)
{
    if (!scene)
    {
        SPDLOG_ERROR("Scene is null during instantiation!");
        return nullptr;
    }

    if (m_nodes.empty())
    {
        SPDLOG_WARN("Model has no nodes to instantiate.");
        return nullptr;
    }

    // 1. 임시 저장소 (인덱스 -> GameObject Raw Pointer)
    // 소유권은 즉시 Scene으로 넘기지만, 부모 연결을 위해 포인터는 가지고 있어야 함.
    std::vector<GameObject*> parts(m_nodes.size(), nullptr);
    GameObject* root = nullptr;

    // 2. 모든 노드 순회하며 생성
    for (usize i = 0; i < m_nodes.size(); ++i)
    {
        const auto& nodeData = m_nodes[i];

        // 2-1. GameObject 생성
        auto go = GameObject::Create();
        go->SetName(nodeData.name);

        // 2-2. Transform 설정 (T-Pose)
        // Transform은 GO 생성 시 자동 생성되므로 GetTransform() 사용
        go->GetTransform().SetLocalMatrix(nodeData.localTransform);

        // 2-3. MeshRenderer 부착
        if (!nodeData.meshIndices.empty())
        {
            for (uint32 meshIdx : nodeData.meshIndices)
            {
                // 편의상 GetMesh 헬퍼가 있다고 가정하거나 직접 접근
                // (여기서는 타입 체크 후 생성 예시)
                if (meshIdx < m_meshes.size())
                {
                    auto& meshRes = m_meshes[meshIdx];
                    // 타입에 따라 Skinned/Static 생성
                    if (meshRes->GetResourceType() == ResourceType::SkinnedMesh)
                    {
                        auto mesh = std::static_pointer_cast<SkinnedMesh>(meshRes);
                        auto renderer = MeshRenderer::Create(mesh, mesh->GetMaterial());
                        go->AddComponent(std::move(renderer));
                    }
                    else
                    {
                        auto mesh = std::static_pointer_cast<StaticMesh>(meshRes);
                        auto renderer = MeshRenderer::Create(mesh, mesh->GetMaterial());
                        go->AddComponent(std::move(renderer));
                    }
                }
            }
        }
        // 2-4. 임시 저장소 등록
        parts[i] = go.get();

        // 2-5. Scene에 소유권 이전 (중요!)
        // 이제 go는 nullptr이 됨
        scene->AddGameObject(std::move(go));
    }

    // 3. 계층 구조 연결 (부모-자식)
    // 모든 GO가 생성된 후에 연결하는 것이 안전함 (인덱스 순서 문제 해결)
    for (usize i = 0; i < m_nodes.size(); ++i)
    {
        const auto& nodeData = m_nodes[i];
        GameObject* currentGO = parts[i];

        if (nodeData.parentIndex >= 0)
        {
            // 부모가 있으면 연결
            GameObject* parentGO = parts[nodeData.parentIndex];
            if (parentGO)
            {
                // GameObject::SetParent -> Transform::SetParent로 위임
                currentGO->SetParent(parentGO);
            }
        }
        else
        {
            // 부모가 없으면 루트
            // (첫 번째 루트만 반환용으로 저장)
            if (!root) root = currentGO;
        }
    }

    return root;
}

/*=================================================================//
//   3d model load process methods : assimp (raw 3d model files)   //
//=================================================================*/
bool Model::LoadByAssimp(const std::string& filename)
{
    Assimp::Importer importer;

    // 1. Import Flags 설정 (ModelConverter와 동일하게 맞춤)
    // 노멀/탄젠트 계산, UV 플립, 뼈 가중치 제한 등 필수 옵션 적용
    const uint32 flags =
        aiProcess_Triangulate |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_LimitBoneWeights;

    const aiScene* scene = importer.ReadFile(filename, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("Failed to load model (Assimp): {}", filename);
        return false;
    }

    // 2. 초기화 (기존 데이터 클리어)
    m_meshes.clear();
    m_materials.clear();
    /*m_boneInfoMap.clear();
    m_BoneCounter = 0;*/
    m_skeleton = Skeleton::Create();

    // 텍스처 로드를 위한 디렉토리 경로 계산
    std::filesystem::path modelDir = std::filesystem::path(filename).parent_path();

    // 3. 머티리얼 처리 (함수 분리)
    ProcessAssimpMaterials(scene, modelDir);

    // 4. 메쉬 처리
    // ProcessAssimpNode(scene->mRootNode, scene);
    m_meshes.reserve(scene->mNumMeshes);
    for (uint32 i = 0; i < scene->mNumMeshes; i++)
        ProcessAssimpMesh(scene->mMeshes[i], scene);

    // 5. 계층 구조 추출 (런타임 생성)
    int32 currentIndex = 0;
    ProcessAssimpHierarchy(scene->mRootNode, -1, currentIndex);

    return true;
}

void Model::ProcessAssimpHierarchy(aiNode* node, int32 parentIndex, int32& currentIndex)
{
    AssetFmt::RawNode rawNode;
    rawNode.name = node->mName.C_Str();
    rawNode.parentIndex = parentIndex;
    rawNode.localTransform = Utils::ConvertToGLMMat4(node->mTransformation);

    // 메쉬 인덱스 정보 복사
    if (node->mNumMeshes > 0)
    {
        rawNode.meshIndices.resize(node->mNumMeshes);
        for (uint32 i = 0; i < node->mNumMeshes; ++i)
            rawNode.meshIndices[i] = node->mMeshes[i];
    }

    int32 myIndex = currentIndex;
    m_nodes.push_back(rawNode);
    currentIndex++;

    for (uint32 i = 0; i < node->mNumChildren; i++)
        ProcessAssimpHierarchy(node->mChildren[i], myIndex, currentIndex);
}

void Model::ProcessAssimpMaterials(const aiScene* scene, const std::filesystem::path& modelDir)
{
    m_materials.resize(scene->mNumMaterials);
    for (uint32 i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* aiMat = scene->mMaterials[i];
        auto material = Material::Create();

        aiColor4D color;
        float value;

        // 1. PBR Factors
        if (aiMat->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS)
            material->albedoFactor = { color.r, color.g, color.b, color.a };
        else if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            material->albedoFactor = { color.r, color.g, color.b, 1.0f };

        // [New] Emissive Color
        if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
            material->emissiveFactor = { color.r, color.g, color.b };

        // [New] Emissive Strength
        if (aiMat->Get(AI_MATKEY_EMISSIVE_INTENSITY, value) == AI_SUCCESS)
            material->emissionStrength = value;
        else
            material->emissionStrength = 1.0f;

        if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
            material->metallicFactor = value;

        if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
            material->roughnessFactor = value;

        // 2. Textures
        auto LoadTex = [&](aiTextureType type) -> TexturePtr 
        {
            return LoadMaterialTexture(aiMat, type, modelDir);
        };

        material->diffuse = LoadTex(aiTextureType_BASE_COLOR);
        if (!material->diffuse) material->diffuse = LoadTex(aiTextureType_DIFFUSE);
        material->normal = LoadTex(aiTextureType_NORMALS);
        material->emission = LoadTex(aiTextureType_EMISSIVE);
        material->metallic = LoadTex(aiTextureType_METALNESS);
        material->roughness = LoadTex(aiTextureType_DIFFUSE_ROUGHNESS);
        material->ao = LoadTex(aiTextureType_AMBIENT_OCCLUSION);
        material->specular = LoadTex(aiTextureType_SPECULAR);
        material->height = LoadTex(aiTextureType_HEIGHT);
        // [Glossiness Fallback]
        auto glossTex = LoadTex(aiTextureType_SHININESS);
        if (!material->roughness && glossTex)
        {
            material->roughness = glossTex;
            material->useGlossinessAsRoughness = true;
        }

        m_materials[i] = std::move(material);
    }
}

void Model::ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene)
{
    bool hasSkeleton = mesh->mNumBones > 0;
    if (hasSkeleton) ProcessAssimpSkinnedMesh(mesh, scene);
    else ProcessAssimpStaticMesh(mesh, scene);
}

void Model::ProcessAssimpSkinnedMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<SkinnedVertex> vertices(mesh->mNumVertices);
    std::vector<uint32> indices;
    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);

    // 1. Vertices
    for (uint32 i = 0; i < mesh->mNumVertices; i++)
    {
        auto& v = vertices[i];
        Skeleton::InitializeVertexBoneData(v);

        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        if (mesh->mTextureCoords[0])  v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        else v.texCoord = { 0.0f, 0.0f };

        if (mesh->mTangents) v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        else v.tangent = { 0.0f, 0.0f, 0.0f };

        minBound = Utils::Min(minBound, v.position);
        maxBound = Utils::Max(maxBound, v.position);
    }

    // 2. Bone Weights (Assimp only)
    ExtractBoneWeightForVertices(vertices, mesh, scene);

    // 3. Indices
    indices.reserve(mesh->mNumFaces * 3);
    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    // 4. Create Mesh
    auto glMesh = SkinnedMesh::Create(vertices, indices, GL_TRIANGLES);
    glMesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));

    if (mesh->mMaterialIndex >= 0 && mesh->mMaterialIndex < m_materials.size())
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

    m_meshes.push_back(std::move(glMesh));
}

void Model::ProcessAssimpStaticMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<StaticVertex> vertices(mesh->mNumVertices);
    std::vector<uint32> indices;

    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);

    // 1. Vertices
    for (uint32 i = 0; i < mesh->mNumVertices; i++)
    {
        auto& v = vertices[i];

        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        if (mesh->mTextureCoords[0]) v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        else v.texCoord = { 0.0f, 0.0f };

        if (mesh->mTangents) v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        else v.tangent = { 0.0f, 0.0f, 0.0f };

        minBound = Utils::Min(minBound, v.position);
        maxBound = Utils::Max(maxBound, v.position);
    }

    // 2. Indices
    indices.reserve(mesh->mNumFaces * 3);
    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    // 3. Create Mesh
    auto glMesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
    glMesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));

    if (mesh->mMaterialIndex >= 0 && mesh->mMaterialIndex < m_materials.size())
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

    m_meshes.push_back(std::move(glMesh));
}

/*========================================================//
//   .mymodel file load process methods : .mymodel file   //
//========================================================*/
bool Model::LoadByBinary(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile)
    {
        SPDLOG_ERROR("Failed to open binary model file (V3): {}", filename);
        return false;
    }

    std::filesystem::path modelDir = std::filesystem::path(filename).parent_path();

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

    SPDLOG_INFO("Model loaded successfully (Binary): {}", filename);
    inFile.close();
    return true;
}

bool Model::ReadBinaryModelHeader(std::ifstream& inFile, uint32& outMatCount, uint32& outMeshCount, bool& outHasSkeleton)
{
    auto magic = AssetUtils::ReadData<uint32>(inFile);
    if (magic != 0x4D594D44)
    {
        SPDLOG_ERROR("Invalid Magic Number");
        return false;
    }

    auto version = AssetUtils::ReadData<uint32>(inFile);
    if (version != 3)
    {
        SPDLOG_WARN("Version mismatch! Expected 2, Got {}", version);
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

void Model::ReadBinaryNodes(std::ifstream& inFile) { m_nodes = AssetUtils::ReadRawNodes(inFile); }

void Model::ReadBinarySkeleton(std::ifstream& inFile)
{
    // 로컬 변수에 먼저 읽어들임
    Skeleton::BoneMap boneMap;
    int32 maxId = 0;

    // 1. 뼈 정보 벡터 읽기 (Loop Read 필수!)
    uint32 bCount = AssetUtils::ReadData<uint32>(inFile);
    std::vector<AssetFmt::RawBoneInfo> rawBoneInfos(bCount);

    for (uint32 i = 0; i < bCount; ++i)
    {
        rawBoneInfos[i].id = AssetUtils::ReadData<uint32>(inFile);
        rawBoneInfos[i].offset = AssetUtils::ReadData<glm::mat4>(inFile);
    }
    maxId = (int32_t)rawBoneInfos.size();

    // 2. 이름 매핑 읽기
    uint32 mapCount = AssetUtils::ReadData<uint32>(inFile);
    for (uint32 i = 0; i < mapCount; ++i)
    {
        std::string name = AssetUtils::ReadString(inFile);
        int32 id = AssetUtils::ReadData<int32>(inFile);

        AssetFmt::RawBoneInfo info;
        info.id = id;
        if (id >= 0 && id < (int32)rawBoneInfos.size())
        {
            info.offset = rawBoneInfos[id].offset;
        }
        boneMap[name] = info;
    }

    // 3. 다 읽은 데이터를 Skeleton에 주입
    if (m_skeleton) m_skeleton->SetData(boneMap, maxId);
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

            auto texPtr = LoadTexture(rawTex.fileName, modelDir);
            if (!texPtr) continue;

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

    for (uint32 i = 0; i < meshCount; ++i)
    {
        // 1. 파일에서 Raw 데이터 덩어리 읽기
        auto rawMesh = AssetUtils::ReadRawMesh(inFile);

        // 2. 타입에 따라 적절한 함수 호출 (분기 처리)
        if (rawMesh.isSkinned)
        {
            CreateBinarySkinnedMesh(rawMesh);
        }
        else
        {
            CreateBinaryStaticMesh(rawMesh);
        }
    }
}

void Model::CreateBinarySkinnedMesh(const AssetFmt::RawMesh& rawMesh)
{
    std::vector<SkinnedVertex> engineVerts(rawMesh.skinnedVertices.size());
    for (size_t v = 0; v < rawMesh.skinnedVertices.size(); ++v)
    {
        const auto& src = rawMesh.skinnedVertices[v];
        auto& dst = engineVerts[v];

        dst.position = src.position;
        dst.normal = src.normal;
        dst.texCoord = src.texCoord;
        dst.tangent = src.tangent;

        for (int j = 0; j < 4; ++j) {
            dst.weights[j] = src.weights[j];
            dst.boneIDs[j] = src.boneIDs[j];
        }
    }

    // 2. 엔진 메쉬 객체 생성
    auto mesh = SkinnedMesh::Create(engineVerts, rawMesh.indices, GL_TRIANGLES);

    // 3. 바운딩 박스 설정
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(rawMesh.aabbMin, rawMesh.aabbMax));
    
    // 4. 머티리얼 연결
    if (rawMesh.materialIndex < m_materials.size())
        mesh->SetMaterial(m_materials[rawMesh.materialIndex]);

    // 5. 등록
    m_meshes.push_back(std::move(mesh));
}

void Model::CreateBinaryStaticMesh(const AssetFmt::RawMesh& rawMesh)
{
    // 1. 정점 변환 (Raw -> Engine)
    std::vector<StaticVertex> engineVerts(rawMesh.staticVertices.size());
    for (size_t v = 0; v < rawMesh.staticVertices.size(); ++v)
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

    // 4. 머티리얼 연결
    if (rawMesh.materialIndex < m_materials.size())
        mesh->SetMaterial(m_materials[rawMesh.materialIndex]);

    // 5. 등록
    m_meshes.push_back(std::move(mesh));
}

/*==========================================//
//   model texture loading helper methods   //
//==========================================*/
TexturePtr Model::LoadMaterialTexture(aiMaterial* material, aiTextureType type, const std::filesystem::path& parentDir)
{
    if (material->GetTextureCount(type) <= 0) return nullptr;

    aiString filepath;
    if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) return nullptr;

    return LoadTexture(filepath.C_Str(), parentDir);
}

TexturePtr Model::LoadTexture(const std::string& path, const std::filesystem::path& parentDir)
{
    if (path.empty()) return nullptr;

    // 1. 파일 이름만 추출 (예: "texture/diffuse.jpg" -> "diffuse.jpg")
    std::string filenameOnly = std::filesystem::path(path).filename().string();
    if (filenameOnly.empty()) return nullptr;

    TexturePtr tex;

    // 2. [최적화] KTX 포맷 로드 시도
    tex = LoadTextureFromKTX(filenameOnly, parentDir);
    if (tex) return tex;

    // 3. [기본] 원본 이미지(png, jpg 등) 로드
    tex = LoadTextureFromImage(filenameOnly, parentDir);
    if (tex) return tex;

    return nullptr;
}

TexturePtr Model::LoadTextureFromKTX(const std::string& filename, const std::filesystem::path& parentDir)
{
    // 1. KTX 경로 구성
    std::filesystem::path ktxPath = parentDir / filename;
    ktxPath.replace_extension(".ktx");
    std::string ktxFullPath = ktxPath.string();

    // 2. 리소스 매니저 캐시 확인
    auto cachedTex = RESOURCE.GetResource<Texture>(ktxFullPath);
    if (cachedTex) return cachedTex;

    // 3. 파일이 실제로 존재하는지 확인 후 로드
    if (std::filesystem::exists(ktxPath))
    {
        auto texture = Texture::CreateFromKtxImage(ktxFullPath);
        if (texture)
        {
            RESOURCE.AddResource<Texture>(std::move(texture), ktxFullPath);
            return RESOURCE.GetResource<Texture>(ktxFullPath);
        }
    }

    return nullptr;
}

TexturePtr Model::LoadTextureFromImage(const std::string& filename, const std::filesystem::path& parentDir)
{
    // 1. 원본 경로 구성
    std::filesystem::path originalPath = parentDir / filename;
    std::string originalFullPath = originalPath.string();

    // 2. 리소스 매니저 캐시 확인
    auto cachedTex = RESOURCE.GetResource<Texture>(originalFullPath);
    if (cachedTex) return cachedTex;

    // 3. 이미지 로드 시도 (Image 클래스 사용)
    auto image = Image::Load(originalFullPath);
    if (!image)
    {
        // 원본조차 없으면 에러 로그 출력
        SPDLOG_ERROR("Failed to load texture image: {}", originalFullPath);
        return nullptr;
    }

    // 4. 텍스처 생성 및 등록
    auto texture = Texture::CreateFromImage(image.get());
    RESOURCE.AddResource<Texture>(std::move(texture), originalFullPath);

    return RESOURCE.GetResource<Texture>(originalFullPath);
}

/*============================//
//   skeleton helper method   //
//============================*/
void Model::ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    // Skeleton이 생성되어 있어야 함
    if (!m_skeleton) return;

    for (uint32 i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();

        // [위임] Skeleton에게 등록 및 ID 발급 요청
        int32 boneID = m_skeleton->AddBone
        (
            boneName,
            Utils::ConvertToGLMMat4(bone->mOffsetMatrix)
        );

        // 웨이트 정보 주입
        for (uint32 w = 0; w < bone->mNumWeights; ++w)
        {
            const auto& weightData = bone->mWeights[w];
            int vertexId = weightData.mVertexId;
            float weight = weightData.mWeight;

            if (vertexId < vertices.size())
                Skeleton::AddBoneWeightToVertex(vertices[vertexId], boneID, weight);
        }
    }
}