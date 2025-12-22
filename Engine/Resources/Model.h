#pragma once
#include "Resources/Resource.h"
#include <filesystem>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics/Vertex.h"
#include "Resources/Bone.h"
#include "Misc/AssetFormat.h"

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

/*=================================================================//
//   3d model load process methods : assimp (raw 3d model files)   //
//=================================================================*/
private:
	bool LoadByAssimp(const std::string& filename);
	void ProcessAssimpNode(aiNode* node, const aiScene* scene);
	void ProcessAssimpMaterials(const aiScene* scene, const std::filesystem::path& modelDir);
	void ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessAssimpSkinnedMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessAssimpStaticMesh(aiMesh* mesh, const aiScene* scene);

/*========================================================//
//   .mymodel file load process methods : .mymodel file   //
//========================================================*/
private:
	bool LoadByBinary(const std::string& filename);
	bool ReadBinaryModelHeader(std::ifstream& inFile, uint32& outMatCount, uint32& outMeshCount, bool& outHasSkeleton);
	void ReadBinarySkeleton(std::ifstream& inFile);
	void ReadBinaryMaterials(std::ifstream& inFile, uint32 matCount, const std::filesystem::path& modelDir);
	void ReadBinaryMeshes(std::ifstream& inFile, uint32 meshCount);
	void CreateBinarySkinnedMesh(const AssetFmt::RawMesh& rawMesh);
	void CreateBinaryStaticMesh(const AssetFmt::RawMesh& rawMesh);

private:
	Model() = default;
	std::vector<MeshPtr> m_meshes;
	std::vector<MaterialPtr> m_materials;

#pragma region SKELETON_SECTION
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

#pragma endregion

#pragma region TEXTURE_LOAD_SECTION
	/*======================//
	//  Texture properties  //
	//======================*/
private:
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
#pragma endregion
};