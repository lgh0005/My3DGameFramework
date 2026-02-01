#pragma once
#include "Resources/Resource.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics/Layouts/Vertex.h"
#include "Resources/Animations/AnimChannel.h"
#include "Misc/AssetFormat.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Animator)
CLASS_PTR(GameObject)
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
	DEFINE_RESOURCE_TYPE(ResourceType::Model)

public:
	virtual ~Model();
	static ModelUPtr Load(const std::string& filename);

	uint32 GetMeshCount() const { return (uint32)m_meshes.size(); }
	SkinnedMeshPtr GetSkinnedMesh(int index) const;
	StaticMeshPtr GetStaticMesh(int index) const;
	SkeletonPtr GetSkeleton() const { return m_skeleton; }
	auto& GetNodes() { return m_nodes; }

/*======================================//
//   3d model load instancing methods   //
//======================================*/
public:
	// 모델을 인스턴스화하여 Scene에 등록하고, Root 오브젝트를 반환
	GameObjectUPtr Instantiate(Scene* scene, Animator* animator = nullptr);

private:
	GameObjectUPtr CreateGameObjectForSingleNode
	(
		Scene* scene,
		GameObject* parent,
		const AssetFmt::RawNode& node,
		Animator* animator
	);

/*=================================================================//
//   3d model load process methods : assimp (raw 3d model files)   //
//=================================================================*/
private:
	bool LoadByAssimp(const std::string& filename);
	void ParseAssimpHierarchy(const aiScene* scene);
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
	void LinkSkeletonHierarchy();
	SkeletonPtr m_skeleton;

private:
	Model();
	std::vector<MeshPtr> m_meshes;
	std::vector<MaterialPtr> m_materials;
	std::vector<uint32> m_meshMaterialIndices;
	std::vector<AssetFmt::RawNode> m_nodes;
};