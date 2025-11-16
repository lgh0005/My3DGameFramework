#include "EnginePch.h"
#include "Model.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Material.h"
#include "Graphics/Image.h"
#include "Graphics/Texture.h"
#include "Graphics/Program.h"

ModelUPtr Model::Load(const std::string& filename)
{
    auto model = ModelUPtr(new Model());

    // 파일 확장명 비교 후 로드 : .mymodel로 로드하는 것을 추천
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

bool Model::LoadByAssimp(const std::string& filename)
{
    Assimp::Importer importer;
    // TODO : 어떤 모델은 UV 좌표가 올바르고 어떤 모델은 뒤집히는 것이 있는 모양이다.
    // auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }

    // TODO : 이후에 한 번 3D 모델 파일들을 작업용 포멧을 한 번 파싱한 다음
    // 그것을 로드하도록 만들어야 한다.
    auto dirname = filename.substr(0, filename.find_last_of("/"));
    auto LoadTexture = [&](aiMaterial* material, aiTextureType type) -> TexturePtr 
    {
        if (material->GetTextureCount(type) <= 0) return nullptr;

        aiString filepath;
        if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) 
            return nullptr;

        std::filesystem::path texturePath(filepath.C_Str());
        std::string filenameOnly = texturePath.filename().string();

        if (filenameOnly.empty())
        {
            SPDLOG_WARN("Texture path was empty: {}", filepath.C_Str());
            return nullptr;
        }

        auto fullPath = (std::filesystem::path(dirname) / filenameOnly).string();
        SPDLOG_INFO("Loading texture [filesystem]: {}", fullPath);

        auto image = Image::Load(fullPath);
        if (!image)
        {
            SPDLOG_WARN("Failed to load image: {}", fullPath);
            return nullptr;
        }

        return Texture::CreateFromImage(image.get());
    };

    for (uint32 i = 0; i < scene->mNumMaterials; i++) 
    {
        auto material = scene->mMaterials[i];
        auto glMaterial = Material::Create();
        glMaterial->diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
        glMaterial->specular = LoadTexture(material, aiTextureType_SPECULAR);
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

    // 2. 머티리얼 로드
    m_materials.resize(materialCount);
    for (uint32 i = 0; i < materialCount; ++i)
    {
        auto material = Material::Create();

        // 텍스처 경로 읽기 헬퍼 람다
        auto ReadPath = [&](std::ifstream& file) -> std::string {
            uint32 len; file.read((char*)&len, sizeof(len));
            std::string path(len, '\0'); file.read(&path[0], len);
            return path;
            };

        // TODO : 텍스쳐 로드 수정 필요
        std::string storedDiffuse = ReadPath(inFile);
        std::string storedSpecular = ReadPath(inFile);

        // TODO: 텍스처 로드 
        // 1. 디퓨즈 맵 로드
        if (!storedDiffuse.empty())
        {
            std::string texFilename = std::filesystem::path(storedDiffuse).filename().string();
            std::string fullPath = (modelDir / texFilename).string();

            auto image = Image::Load(fullPath);
            if (image)
                material->diffuse = Texture::CreateFromImage(image.get());
            else
                SPDLOG_ERROR("Failed to load diffuse texture: '{}'", fullPath);
        }

        // 2. 스페큘러 맵 로드
        if (!storedSpecular.empty())
        {
            std::string texFilename = std::filesystem::path(storedSpecular).filename().string();
            std::string fullPath = (modelDir / texFilename).string();

            auto image = Image::Load(fullPath);
            if (image)
                material->specular = Texture::CreateFromImage(image.get());
            else
                SPDLOG_ERROR("Failed to load specular texture: '{}'", fullPath);
        }
        
        // TODO : 이후 여러 텍스쳐 맵이 필요하다면 추가될 수 있음

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

        // 데이터 통째로 읽기
        std::vector<SkinnedVertex> vertices(vertexCount);
        std::vector<uint32> indices(indexCount);
        inFile.read((char*)vertices.data(), sizeof(SkinnedVertex) * vertexCount);
        inFile.read((char*)indices.data(), sizeof(uint32) * indexCount);

        // GPU 버퍼 생성
        auto mesh = SkinnedMesh::Create(vertices, indices, GL_TRIANGLES);
        if (materialIndex < m_materials.size())
        {
            mesh->SetMaterial(m_materials[materialIndex]);
        }
        m_meshes.push_back(std::move(mesh));
    }

    inFile.close();
    SPDLOG_INFO("Loaded custom model: {} ({} meshes, {} bones)",
        filename, meshCount, m_boneInfoMap.size());
    return true;
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}",
        mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<SkinnedVertex> vertices;
    vertices.resize(mesh->mNumVertices);
    for (uint32 i = 0; i < mesh->mNumVertices; i++) 
    {
        auto& v = vertices[i];

        // 뼈 데이터를 기본값(-1)으로 초기화
        SetVertexBoneDataToDefault(v);
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }

    std::vector<uint32> indices;
    indices.resize(mesh->mNumFaces * 3);
    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        indices[3 * i] = mesh->mFaces[i].mIndices[0];
        indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    // 뼈 데이터 추출 및 정점 벡터 업데이트
    ExtractBoneWeightForVertices(vertices, mesh, scene);

    auto glMesh = SkinnedMesh::Create(vertices, indices, GL_TRIANGLES);
    if (mesh->mMaterialIndex >= 0)
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);
    m_meshes.push_back(std::move(glMesh));
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
            newBoneInfo.offset = Utils::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
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
