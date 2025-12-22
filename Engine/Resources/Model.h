#pragma once
#include "Resources/Resource.h"
#include <filesystem>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics/Vertex.h"
#include "Resources/Bone.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(SkinnedMesh)
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Model)
class Model : public Resource
{
public:
	static const ResourceType s_ResourceType = ResourceType::Model;
	virtual ResourceType GetResourceType() const override { return ResourceType::Model; }
	static ModelUPtr Load(const std::string& filename);
	uint32 GetMeshCount() const { return (uint32)m_meshes.size(); }
	SkinnedMeshPtr GetSkinnedMesh(int index) const;
	StaticMeshPtr GetStaticMesh(int index) const;
	void Draw(const Program* program) const;

/*===================//
//  Bone properties  //
//===================*/
public:
	auto& GetBoneInfoMap() { return m_boneInfoMap; }
	int32& GetBoneCount() { return m_BoneCounter; }

private:
	void SetVertexBoneDataToDefault(SkinnedVertex& vertex);
	void SetVertexBoneData(SkinnedVertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices,
								 aiMesh* mesh, const aiScene* scene);

private:
	std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
	int32 m_BoneCounter = 0;

private:
	Model() = default;

#pragma region LEGACY_ASSET_CONVERTER_METHODS
	bool LoadByAssimp(const std::string& filename);
	bool LoadByBinary(const std::string& filename);
#pragma endregion

#pragma region TEST_FOR_ASSET_CONVERTER
	bool LoadByAssimpV2(const std::string& filename);
	bool LoadByBinaryV2(const std::string& filename);
#pragma endregion

#pragma region TEST_FOR_ASSET_CONVERTER_2
	bool LoadByBinaryV3(const std::string& filename);
#pragma endregion

	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessNode(aiNode* node, const aiScene* scene);
	TexturePtr LoadTextureFromFile
	(
		const std::string& relativePath, 
		const std::filesystem::path& parentDir
	);
	TexturePtr LoadTextureFromAssimp
	(
		aiMaterial* material, 
		aiTextureType type, 
		const std::filesystem::path& parentDir
	);
	std::vector<MeshPtr> m_meshes;
	std::vector<MaterialPtr> m_materials;
};