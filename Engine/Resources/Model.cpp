#include "EnginePch.h"
#include "Model.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Image.h"
#include "Resources/Texture.h"
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
    m_boneInfoMap.clear();
    m_BoneCounter = 0;

    // 텍스처 로드를 위한 디렉토리 경로 계산
    std::filesystem::path modelDir = std::filesystem::path(filename).parent_path();

    // 3. 머티리얼 처리 (함수 분리)
    ProcessAssimpMaterials(scene, modelDir);

    // 4. 노드 및 메쉬 처리 (함수 분리)
    ProcessAssimpNode(scene->mRootNode, scene);

    return true;
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
            return LoadTextureFromAssimp(aiMat, type, modelDir);
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

void Model::ProcessAssimpNode(aiNode* node, const aiScene* scene)
{
    for (uint32 i = 0; i < node->mNumMeshes; i++)
    {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessAssimpMesh(mesh, scene);
    }

    for (uint32 i = 0; i < node->mNumChildren; i++)
        ProcessAssimpNode(node->mChildren[i], scene);
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
        SetVertexBoneDataToDefault(v);

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

    // 2. Skeleton Read
    if (hasSkeleton) ReadBinarySkeleton(inFile);

    // 3. Materials Read
    ReadBinaryMaterials(inFile, matCount, modelDir);

    // 4. Meshes Read
    ReadBinaryMeshes(inFile, meshCount);

    SPDLOG_INFO("Model loaded successfully (Binary): {}", filename);
    inFile.close();
    return true;
}

bool Model::ReadBinaryModelHeader(std::ifstream& inFile, uint32& outMatCount, uint32& outMeshCount, bool& outHasSkeleton)
{
    auto magic = AssetUtils::ReadData<uint32_t>(inFile);
    if (magic != 0x4D594D44)
    {
        SPDLOG_ERROR("Invalid Magic Number");
        return false;
    }

    auto version = AssetUtils::ReadData<uint32_t>(inFile);
    if (version != 2)
    {
        SPDLOG_WARN("Version mismatch! Expected 2, Got {}", version);
    }

    // 값들을 읽어서 참조 인자에 할당
    outMatCount = AssetUtils::ReadData<uint32_t>(inFile);
    outMeshCount = AssetUtils::ReadData<uint32_t>(inFile);
    outHasSkeleton = AssetUtils::ReadData<bool>(inFile);

    // 전역 AABB는 현재 Model 클래스 멤버가 아니면 여기서 읽고 버리거나, 멤버에 저장합니다.
    auto globalMin = AssetUtils::ReadData<glm::vec3>(inFile);
    auto globalMax = AssetUtils::ReadData<glm::vec3>(inFile);

    return true;
}

void Model::ReadBinarySkeleton(std::ifstream& inFile)
{
    // 1. 뼈 정보 벡터 읽기 (Loop Read 필수!)
    uint32_t bCount = AssetUtils::ReadData<uint32_t>(inFile);
    std::vector<AssetFmt::RawBoneInfo> rawBoneInfos(bCount);

    for (uint32_t i = 0; i < bCount; ++i)
    {
        rawBoneInfos[i].id = AssetUtils::ReadData<uint32_t>(inFile);
        rawBoneInfos[i].offset = AssetUtils::ReadData<glm::mat4>(inFile);
    }

    m_BoneCounter = (int32_t)rawBoneInfos.size();

    // 2. 이름 매핑 읽기
    uint32_t mapCount = AssetUtils::ReadData<uint32_t>(inFile);
    for (uint32_t i = 0; i < mapCount; ++i)
    {
        std::string name = AssetUtils::ReadString(inFile);
        int32_t id = AssetUtils::ReadData<int32_t>(inFile);

        BoneInfo info;
        info.id = id;
        if (id >= 0 && id < (int32)rawBoneInfos.size())
        {
            info.offset = rawBoneInfos[id].offset;
        }
        m_boneInfoMap[name] = info;
    }
}

void Model::ReadBinaryMaterials(std::ifstream& inFile, uint32 matCount, const std::filesystem::path& modelDir)
{
    m_materials.resize(matCount);
    for (uint32_t i = 0; i < matCount; ++i)
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

            auto texPtr = LoadTextureFromFile(rawTex.fileName, modelDir);
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

    for (uint32_t i = 0; i < meshCount; ++i)
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

/*=================//
//   TODOs later   //
//=================*/
#pragma region SKELETON_SECTION
/*===================//
//  Bone properties  //
//===================*/
void Model::SetVertexBoneDataToDefault(SkinnedVertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1; // -1로 초기화
        vertex.weights[i] = 0.0f;
    }
}

void Model::SetVertexBoneData(SkinnedVertex& vertex, int boneID, float weight)
{
    // -1로 초기화된 첫 번째 빈 슬롯을 찾음
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.boneIDs[i] < 0) // -1인 슬롯 발견
        {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;
            break; // 찾았으면 루프 중단
        }
    }
}

void Model::ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices,
                                    aiMesh* mesh, const aiScene* scene)
{
    // 1. 이 메쉬의 모든 뼈를 순회
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        // 2. 이 뼈가 맵에 등록된 뼈인지 확인 (unordered_map 사용)
        if (m_boneInfoMap.find(boneName) == m_boneInfoMap.end())
        {
            // 3. 새 뼈라면, 새 ID와 오프셋을 맵에 등록
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = Utils::ConvertToGLMMat4(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_boneInfoMap[boneName] = newBoneInfo;

            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
            // 4. 이미 등록된 뼈라면 ID만 가져옴
            boneID = m_boneInfoMap[boneName].id;
        }
        // assert(boneID != -1);

        // 5. 이 뼈가 영향을 주는 모든 정점(Weight)을 순회
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            // assert(vertexId < vertices.size()); // (등호 포함X)

            // 6. 'vertices' 벡터에 있는 실제 정점에 뼈 데이터 주입
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}
#pragma endregion

#pragma region TEXTURE_LOAD_SECTION

/*========================//
//  texture load methods  //
//========================*/
TexturePtr Model::LoadTextureFromFile
(
    const std::string& relativePath, // TODO : 이는 상대 경로가 아닌 순수 모델 파일명이 될 것임.
    const std::filesystem::path& parentDir
)
{
    if (relativePath.empty()) return nullptr;
    
    // 파일 이름만 추출
    std::string filenameOnly = std::filesystem::path(relativePath).filename().string();
    if (filenameOnly.empty()) return nullptr;

    /*=======================//
    //  ktx file extraction  //
    //=======================*/
    // 1. KTX 경로 구성 (예: .../wood.ktx)
    std::filesystem::path ktxPath = parentDir / filenameOnly;
    ktxPath.replace_extension(".ktx");
    std::string ktxFullPath = ktxPath.string();

    // 2. KTX 캐시 확인
    auto cachedTex = RESOURCE.GetResource<Texture>(ktxFullPath);
    if (cachedTex) return cachedTex;

    // 3. KTX 파일 존재 여부 확인 및 로드
    if (std::filesystem::exists(ktxPath))
    {
        auto texture = Texture::CreateFromKtxImage(ktxFullPath);
        if (texture)
        {
            RESOURCE.AddResource<Texture>(std::move(texture), ktxFullPath);
            return RESOURCE.GetResource<Texture>(ktxFullPath);
        }
    }

    /*===========================//
    //  png/jpg file extraction  //
    //===========================*/
    std::filesystem::path originalPath = parentDir / filenameOnly;
    std::string originalFullPath = originalPath.string();

    // 원래 경로로도 캐시 확인
    cachedTex = RESOURCE.GetResource<Texture>(originalFullPath);
    if (cachedTex) return cachedTex;

    // 이미지 로드 시도
    auto image = Image::Load(originalFullPath);
    if (!image)
    {
        SPDLOG_ERROR("Failed to load texture image: {}", originalFullPath);
        return nullptr;
    }

    auto texture = Texture::CreateFromImage(image.get());
    RESOURCE.AddResource<Texture>(std::move(texture), originalFullPath);
    return RESOURCE.GetResource<Texture>(originalFullPath);
}

TexturePtr Model::LoadTextureFromAssimp
(
    aiMaterial* material, 
    aiTextureType type, 
    const std::filesystem::path& parentDir
)
{
    if (material->GetTextureCount(type) <= 0) return nullptr;

    aiString filepath;
    if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) return nullptr;

    return LoadTextureFromFile(filepath.C_Str(), parentDir);
}

#pragma endregion
