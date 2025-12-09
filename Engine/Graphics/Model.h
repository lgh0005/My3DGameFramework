#pragma once
#include <filesystem>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics/Vertex.h"
#include "Graphics/Bone.h"

// TODO :
// IMPORTANT : 지금 이 Model 클래스는 움직이는 모델 한정에서만
// 로드를 지원하고 있다. 정적 물체에 대한 로드도 지원할 수 있도록
// 수정이 필요하다.

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(SkinnedMesh)
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(Model)
class Model
{
public:
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
	bool LoadByAssimp(const std::string& filename);
	bool LoadByBinary(const std::string& filename);
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