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