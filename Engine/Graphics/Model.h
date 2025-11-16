#pragma once
#include <filesystem>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics/Vertex.h"
#include "Graphics/Bone.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SkinnedMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(Model)
class Model
{
public:
	static ModelUPtr Load(const std::string& filename);
	uint32 GetMeshCount() const { return (uint32)m_meshes.size(); }
	SkinnedMeshPtr GetMesh(int index) const { return m_meshes[index]; }
	void Draw(const Program* program) const;

/*===================//
//  Bone properties  //
//===================*/
public: // TODO : public으로 열어둬야 하는거 
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
	std::vector<SkinnedMeshPtr> m_meshes;
	std::vector<MaterialPtr> m_materials;
};