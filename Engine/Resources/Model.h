#pragma once
#include "Resources/Resource.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics/Vertex.h"
#include "Resources/AnimChannel.h"
#include "Misc/AssetFormat.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(SkinnedMesh)
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
CLASS_PTR(Texture)
CLASS_PTR(Skeleton)
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
	SkeletonPtr GetSkeleton() const { return m_skeleton; }
	auto& GetNodes() { return m_nodes; }

	void Draw(const Program* program) const;

/*=================================================================//
//   3d model load process methods : assimp (raw 3d model files)   //
//=================================================================*/
private:
	bool LoadByAssimp(const std::string& filename);
	void ProcessAssimpNode(aiNode* node, const aiScene* scene);
	void ProcessAssimpHierarchy(aiNode* node, int32 parentIndex, int32& currentIndex);
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
	void ReadBinaryNodes(std::ifstream& inFile);
	void ReadBinarySkeleton(std::ifstream& inFile);
	void ReadBinaryMaterials(std::ifstream& inFile, uint32 matCount, const std::filesystem::path& modelDir);
	void ReadBinaryMeshes(std::ifstream& inFile, uint32 meshCount);
	void CreateBinarySkinnedMesh(const AssetFmt::RawMesh& rawMesh);
	void CreateBinaryStaticMesh(const AssetFmt::RawMesh& rawMesh);

/*==========================================//
//   model texture loading helper methods   //
//==========================================*/
private:
	TexturePtr LoadMaterialTexture(aiMaterial* material, aiTextureType type, const std::filesystem::path& parentDir);
	TexturePtr LoadTexture(const std::string& path, const std::filesystem::path& parentDir);
	TexturePtr LoadTextureFromKTX(const std::string& filename, const std::filesystem::path& parentDir);
	TexturePtr LoadTextureFromImage(const std::string& filename, const std::filesystem::path& parentDir);

/*============================//
//   skeleton helper method   //
//============================*/
private:
	void ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, aiMesh* mesh, const aiScene* scene);
	SkeletonPtr m_skeleton;

private:
	Model() = default;
	std::vector<MeshPtr> m_meshes;
	std::vector<MaterialPtr> m_materials;
	std::vector<AssetFmt::RawNode> m_nodes;
};