#include "EnginePch.h"
#include "Model.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Material.h"
#include "Graphics/Image.h"
#include "Graphics/Texture.h"
#include "Graphics/Program.h"

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

    // 파일 확장명 비교 후 로드 : .mymodel로 로드하는 것을 추천
    std::string ext = std::filesystem::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".mymodel")
    {
        if (!model->LoadByBinaryV2(filename)) return nullptr;
        // if (!model->LoadByBinary(filename)) return nullptr;
    }
    else
    {
        if (!model->LoadByAssimp(filename)) return nullptr;
    }

    return std::move(model);
}

bool Model::LoadByAssimp(const std::string& filename)
{
    Assimp::Importer importer;
    // INFO : 어떤 모델은 UV 좌표가 올바르고 어떤 모델은 뒤집히는 것이 있는 모양이다.
    // 텍스쳐가 이상하게 뒤집한다면 파일 단에서 수정을 한 후 적용한다.
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }

    std::filesystem::path modelDir = std::filesystem::path(filename).parent_path();

    m_materials.clear();
    for (uint32 i = 0; i < scene->mNumMaterials; i++) 
    {
        auto material = scene->mMaterials[i];
        auto glMaterial = Material::Create();
        glMaterial->diffuse = LoadTextureFromAssimp(material, aiTextureType_DIFFUSE, modelDir);
        glMaterial->specular = LoadTextureFromAssimp(material, aiTextureType_SPECULAR, modelDir);
        glMaterial->emission = LoadTextureFromAssimp(material, aiTextureType_EMISSIVE, modelDir);
        glMaterial->normal = LoadTextureFromAssimp(material, aiTextureType_NORMALS, modelDir);
        glMaterial->metallic = LoadTextureFromAssimp(material, aiTextureType_METALNESS, modelDir);
        glMaterial->roughness = LoadTextureFromAssimp(material, aiTextureType_DIFFUSE_ROUGHNESS, modelDir);
        glMaterial->ao = LoadTextureFromAssimp(material, aiTextureType_AMBIENT_OCCLUSION, modelDir);

        // IFNO : obj를 로드할 때 normal을 height에 넣어야 작동되는 경우가 있음.
        // 그에 대한 방어 코드를 작성
        // TODO : 간혹 Diffuse맵만 상하를 반전시켜야 올바르게 로드가 될 수 있음.
        // 텍스쳐에 대한 것은 이미지를 적절히 뒤집에서 UV를 맞춰줄 필요가 있음.
#pragma region OBJ_EXTENSION_COMPATIBLE
        // Normal을 aiTextureType_HEIGHT로 우선 로드
        if (!glMaterial->normal)
        {
            glMaterial->normal = LoadTextureFromAssimp(material, aiTextureType_HEIGHT, modelDir);
        }

        // 그 후, Height 로드
        TexturePtr tempHeight = LoadTextureFromAssimp(material, aiTextureType_HEIGHT, modelDir);
        if (tempHeight != glMaterial->normal)
        {
            glMaterial->height = tempHeight;
        }
        else
        {
            glMaterial->height = nullptr; // 납치됐으면 Height 슬롯은 비워줌
        }
#pragma endregion

        if (glMaterial->emission) glMaterial->emissionStrength = 1.0f;
        else glMaterial->emissionStrength = 0.0f;

        if (!glMaterial->height) glMaterial->heightScale = 0.0f;
        else glMaterial->heightScale = 0.05f;

        m_materials.push_back(std::move(glMaterial));
    }

    ProcessNode(scene->mRootNode, scene);
    return true;
}

bool Model::LoadByBinary(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) 
    { 
        SPDLOG_ERROR("Failed to open model file: {}", filename); 
        return false; 
    }
    
    // [핵심] 현재 로드하는 .mymodel 파일의 디렉터리 경로를 구합니다.
    // 예: "./Resources/Models/backpack/"
    std::filesystem::path modelDir = std::filesystem::path(filename).parent_path();

    // [약속된 순서대로 읽기]
    uint32 magic, version, materialCount, meshCount;
    bool hasSkeleton;
    inFile.read((char*)&magic, sizeof(magic));
    inFile.read((char*)&version, sizeof(version));
    inFile.read((char*)&materialCount, sizeof(materialCount));
    inFile.read((char*)&meshCount, sizeof(meshCount));
    inFile.read((char*)&hasSkeleton, sizeof(hasSkeleton));

    // 1. 스켈레톤 로드
    if (hasSkeleton)
    {
        uint32 boneCount;
        inFile.read((char*)&boneCount, sizeof(boneCount));
        for (uint32 i = 0; i < boneCount; ++i)
        {
            uint32 nameLen;
            inFile.read((char*)&nameLen, sizeof(nameLen));
            std::string boneName(nameLen, '\0');
            inFile.read(&boneName[0], nameLen);

            BoneInfo info;
            inFile.read((char*)&info, sizeof(BoneInfo));
            m_boneInfoMap[boneName] = info;
        }
        m_BoneCounter = (int32)boneCount; // 카운터 갱신
    }

    // 헬퍼: 바이너리 문자열 읽기
    auto ReadPath = [&](std::ifstream& file) -> std::string {
        uint32 len; file.read((char*)&len, sizeof(len));
        if (len == 0) return "";
        std::string path(len, '\0'); file.read(&path[0], len);
        return path;
        };

    // 2. 머티리얼 로드
    // TODO : PBR 관련 텍스쳐도 로드해야 함.
    m_materials.resize(materialCount);
    for (uint32 i = 0; i < materialCount; ++i)
    {
        auto material = Material::Create();

        // 파일에서 경로 문자열 읽기
        std::string storedDiffuse = ReadPath(inFile);
        std::string storedSpecular = ReadPath(inFile);
        std::string storedEmission = ReadPath(inFile);
        std::string storedNormal = ReadPath(inFile);
        std::string storedHeight = ReadPath(inFile);

        // 위에서 만든 헬퍼 함수로 깔끔하게 로드
        material->diffuse = LoadTextureFromFile(storedDiffuse, modelDir);
        material->specular = LoadTextureFromFile(storedSpecular, modelDir);
        material->emission = LoadTextureFromFile(storedEmission, modelDir);
        material->normal = LoadTextureFromFile(storedNormal, modelDir);
        material->height = LoadTextureFromFile(storedHeight, modelDir);

        // 파라미터 설정 (기존 로직 유지)
        if (material->emission) material->emissionStrength = 1.0f;
        else material->emissionStrength = 0.0f;

        if (material->height) material->heightScale = 0.05f;
        else material->heightScale = 0.0f;

        m_materials[i] = std::move(material);
    }

    // 3. 메쉬 로드
    m_meshes.reserve(meshCount);
    for (uint32 i = 0; i < meshCount; ++i)
    {
        uint32 materialIndex, vertexCount, indexCount;
        inFile.read((char*)&materialIndex, sizeof(materialIndex));
        inFile.read((char*)&vertexCount, sizeof(vertexCount));
        inFile.read((char*)&indexCount, sizeof(indexCount));

        std::vector<uint32> indices(indexCount);

        if (hasSkeleton)
        {
            // 뼈가 있어서 애니메이션이 가능한 모델
            std::vector<SkinnedVertex> vertices(vertexCount);
            inFile.read((char*)vertices.data(), sizeof(SkinnedVertex) * vertexCount);
            inFile.read((char*)indices.data(), sizeof(uint32) * indexCount);

            auto mesh = SkinnedMesh::Create(vertices, indices, GL_TRIANGLES);
            if (materialIndex < m_materials.size()) mesh->SetMaterial(m_materials[materialIndex]);
            m_meshes.push_back(std::move(mesh));
        }
        else
        {
            // 뼈가 없어서 움직이지 않는 정적 모델
            std::vector<StaticVertex> vertices(vertexCount);
            inFile.read((char*)vertices.data(), sizeof(StaticVertex)* vertexCount);
            inFile.read((char*)indices.data(), sizeof(uint32)* indexCount);

            auto mesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
            if (materialIndex < m_materials.size()) mesh->SetMaterial(m_materials[materialIndex]);
            m_meshes.push_back(std::move(mesh));
        }
    }

    inFile.close();
    return true;
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);

    // 인덱스 추출
    std::vector<uint32> indices;
    indices.resize(mesh->mNumFaces * 3);
    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        indices[3 * i] = mesh->mFaces[i].mIndices[0];
        indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    if (mesh->mNumBones > 0)
    {
        // 뼈가 있어서 애니메이션이 가능한 모델
        std::vector<SkinnedVertex> vertices;
        vertices.resize(mesh->mNumVertices);

        for (uint32 i = 0; i < mesh->mNumVertices; i++)
        {
            auto& v = vertices[i];
            SetVertexBoneDataToDefault(v);

            v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0])
            {
                v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
            {
                v.texCoord = glm::vec2(0.0f);
            }

            if (mesh->mTangents)
            {
                v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            }
            else
            {
                v.tangent = glm::vec3(0.0f);
            }

            minBound = Utils::Min(minBound, v.position);
            maxBound = Utils::Max(maxBound, v.position);
        }

        // 뼈 가중치 추출 (정적 메쉬엔 없음)
        ExtractBoneWeightForVertices(vertices, mesh, scene);

        auto glMesh = SkinnedMesh::Create(vertices, indices, GL_TRIANGLES);
        glMesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));
        if (mesh->mMaterialIndex >= 0) glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

        m_meshes.push_back(std::move(glMesh));
    }
    else
    {
        // 뼈가 없어서 움직이지 않는 정적 모델
        std::vector<StaticVertex> vertices;
        vertices.resize(mesh->mNumVertices);

        for (uint32 i = 0; i < mesh->mNumVertices; i++)
        {
            auto& v = vertices[i];
            v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0])
                v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                v.texCoord = glm::vec2(0.0f);

            if (mesh->mTangents)
            {
                v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            }
            else
            {
                v.tangent = glm::vec3(0.0f);
            }

            minBound = Utils::Min(minBound, v.position);
            maxBound = Utils::Max(maxBound, v.position);
        }

        auto glMesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
        glMesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));
        if (mesh->mMaterialIndex >= 0) glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

        m_meshes.push_back(std::move(glMesh));
    }
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32 i = 0; i < node->mNumMeshes; i++) 
    {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene);
    }

    for (uint32 i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], scene);
}

void Model::Draw(const Program* program) const
{
    for (auto& mesh : m_meshes) mesh->Draw(program);
}

#pragma region TEST_FOR_ASSET_CONVERTER

bool Model::LoadByBinaryV2(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile)
    {
        SPDLOG_ERROR("Failed to open binary model file: {}", filename);
        return false;
    }

    std::filesystem::path modelDir = std::filesystem::path(filename).parent_path();

    // 헬퍼: 바이너리 문자열 읽기
    // TODO : Utils 함수로 따로 뺄 필요가 있음
    auto ReadString = [&](std::ifstream& file) -> std::string 
    {
        uint32 len = 0;
        file.read((char*)&len, sizeof(len));
        if (len == 0) return "";
        std::string str(len, '\0');
        file.read(&str[0], len);
        return str;
    };

    // TODO : 이 일련의 과정들을 조금 씩 함수로 빼서 이 함수의 부피를 줄일 필요가 
    // 있어 보인다.

    // 1. 헤더 읽기 (Header)
    uint32 magic, version, materialCount, meshCount;
    bool hasSkeleton;
    glm::vec3 globalMin, globalMax;

    inFile.read((char*)&magic, sizeof(magic));
    if (magic != 0x4D594D44) { SPDLOG_ERROR("Invalid Magic Number"); return false; }
    
    inFile.read((char*)&version, sizeof(version));
    if (version != 2) { SPDLOG_WARN("Version mismatch! Expected 2, Got {}", version); }

    inFile.read((char*)&materialCount, sizeof(materialCount));
    inFile.read((char*)&meshCount, sizeof(meshCount));
    inFile.read((char*)&hasSkeleton, sizeof(hasSkeleton));

    inFile.read((char*)&globalMin, sizeof(glm::vec3));
    inFile.read((char*)&globalMax, sizeof(glm::vec3));

    // 2. 스켈레톤 로드 (Skeleton Info)
    if (hasSkeleton)
    {
        uint32 boneCount;
        inFile.read((char*)&boneCount, sizeof(boneCount));
        for (uint32 i = 0; i < boneCount; ++i)
        {
            std::string boneName = ReadString(inFile);
            BoneInfo info;
            inFile.read((char*)&info, sizeof(BoneInfo));
            m_boneInfoMap[boneName] = info;
        }
        m_BoneCounter = (int32)boneCount;
    }

    // 3. 머티리얼 로드 (Materials)
    m_materials.resize(materialCount);
    for (uint32 i = 0; i < materialCount; ++i) 
    {
        auto material = Material::Create();

        // [순서 엄수] 텍스처 경로 8개 읽기
        std::string pathAlbedo    = ReadString(inFile);
        std::string pathNormal    = ReadString(inFile);
        std::string pathEmissive  = ReadString(inFile);
        std::string pathORM       = ReadString(inFile); // V2
        std::string pathRoughness = ReadString(inFile); // V2
        std::string pathMetallic  = ReadString(inFile); // V2
        std::string pathSpecular  = ReadString(inFile); // Legacy
        std::string pathHeight    = ReadString(inFile); // Legacy

        // PBR Factors 읽기 (사용 안 하더라도 읽어서 스킵해야 함)
        glm::vec4 albedoFactor;
        float metallicFactor, roughnessFactor;
        inFile.read((char*)&albedoFactor, sizeof(glm::vec4));
        inFile.read((char*)&metallicFactor, sizeof(float));
        inFile.read((char*)&roughnessFactor, sizeof(float));

        // 실제 텍스처 로드
        material->diffuse = LoadTextureFromFile(pathAlbedo, modelDir);
        material->normal = LoadTextureFromFile(pathNormal, modelDir);
        material->specular = LoadTextureFromFile(pathSpecular, modelDir); // Legacy
        material->emission = LoadTextureFromFile(pathEmissive, modelDir);
        material->metallic = LoadTextureFromFile(pathMetallic, modelDir);
        material->roughness = LoadTextureFromFile(pathRoughness, modelDir);
        if (!pathORM.empty()) material->ao = LoadTextureFromFile(pathORM, modelDir);
        else material->ao = nullptr; // 혹은 별도 AO 경로가 있다면 그것 사용
        material->height = LoadTextureFromFile(pathHeight, modelDir);

        m_materials[i] = std::move(material);
    }

    // 4. 메쉬 로드 (Meshes)
    m_meshes.reserve(meshCount);
    for (uint32 i = 0; i < meshCount; ++i)
    {
        uint32 materialIndex;
        bool isSkinned;
        glm::vec3 aabbMin, aabbMax;

        // 메쉬 헤더 읽기
        inFile.read((char*)&materialIndex, sizeof(materialIndex));
        inFile.read((char*)&isSkinned, sizeof(isSkinned));
        inFile.read((char*)&aabbMin, sizeof(aabbMin));
        inFile.read((char*)&aabbMax, sizeof(aabbMax));

        uint32 vCount, iCount;
        inFile.read((char*)&vCount, sizeof(vCount));
        inFile.read((char*)&iCount, sizeof(iCount));

        std::vector<uint32> indices(iCount);

        if (isSkinned)
        {
            // [SkinnedMesh 생성]
            std::vector<SkinnedVertex> vertices(vCount);
            inFile.read((char*)vertices.data(), sizeof(SkinnedVertex) * vCount);
            inFile.read((char*)indices.data(), sizeof(uint32) * iCount);

            auto mesh = SkinnedMesh::Create(vertices, indices, GL_TRIANGLES);
            mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(aabbMin, aabbMax));
            if (materialIndex < m_materials.size()) mesh->SetMaterial(m_materials[materialIndex]);
            m_meshes.push_back(std::move(mesh));
        }
        else
        {
            // [StaticMesh 생성]
            std::vector<StaticVertex> vertices(vCount);
            inFile.read((char*)vertices.data(), sizeof(StaticVertex) * vCount);
            inFile.read((char*)indices.data(), sizeof(uint32) * iCount);

            auto mesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
            mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(aabbMin, aabbMax));
            if (materialIndex < m_materials.size()) mesh->SetMaterial(m_materials[materialIndex]);
            m_meshes.push_back(std::move(mesh));
        }
    }

    inFile.close();
    return true;
}

#pragma endregion

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
            RESOURCE.AddResource<Texture>(ktxFullPath, std::move(texture));
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
    RESOURCE.AddResource<Texture>(originalFullPath, std::move(texture));
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
