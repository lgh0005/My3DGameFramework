#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(Model)
class Model
{
public:
	static ModelUPtr Load(const std::string& filename);
	uint32 GetMeshCount() const { return (uint32)m_meshes.size(); }
	MeshPtr GetMesh(int index) const { return m_meshes[index]; }
	void Draw(const Program* program) const;

private:
	Model() = default;
	bool LoadByAssimp(const std::string& filename);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessNode(aiNode* node, const aiScene* scene);
	std::vector<MeshPtr> m_meshes;
	std::vector<MaterialPtr> m_materials;
};