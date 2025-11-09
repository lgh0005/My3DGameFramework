#include "EnginePch.h"
#include "Model.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics//Image.h"
#include "Graphics/Texture.h"
#include "Graphics/Program.h"

ModelUPtr Model::Load(const std::string& filename)
{
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename)) return nullptr;
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string& filename)
{
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
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

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}",
        mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices;
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

    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
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
void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1; // -1로 초기화
        vertex.weights[i] = 0.0f;
    }
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
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

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, 
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
