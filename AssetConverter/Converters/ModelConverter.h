#pragma once
#include "AssetTypes/AssetFormat.h"

class ModelConverter
{
	DECLARE_SINGLE(ModelConverter)

public:
	bool Convert(const std::string& inputPath, 
				 const std::string& outputPath, bool extractORM);

/*============================//
//   main coversion methods   //
//============================*/
private:
	bool RunConversion();
	bool WriteCustomModelFile();
	void CreateORMTextureFromAssimp(aiMaterial* material, AssetFmt::RawMaterial& rawMat, int32 index);

	// 모델 데이터 구조체
	AssetFmt::RawModel m_rawModel;

	// 경로 문자열 멤버
	std::string m_inputPath;
	std::string m_outputPath;
	std::string m_modelDirectory;
	std::string m_modelName;

/*====================================//
//   default assimp process methods   //
//====================================*/
private:
	void ParseNodeHierarchy(const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessSkinnedMesh(aiMesh* mesh, AssetFmt::RawMesh& rawMesh);
	void ProcessStaticMesh(aiMesh* mesh, AssetFmt::RawMesh& rawMesh);
	AssetFmt::RawMaterial ProcessMaterial(aiMaterial* material, int32 index);
	bool m_extractORM{ false };

/*=========================================================//
//   texture helper methods for material and ORM texture   //
//=========================================================*/
private:
	std::string ResolveTexturePath(const std::string& relativePath);
	void AddTextureToMaterial(AssetFmt::RawMaterial& rawMat, aiMaterial* aiMat, aiTextureType aiType, AssetFmt::RawTextureType rawType);
	std::string GetTexturePath(aiMaterial* material, aiTextureType type);
	void LogFinalMappedTextures(aiMaterial* material, const AssetFmt::RawMaterial& rawMat);
	void ProcessTextureToKTX(AssetFmt::RawMaterial& rawMat, const std::string& srcFileName, AssetFmt::RawTextureType type);

	std::unordered_set<std::string> m_convertedTextures;

/*==========================================//
//   skeleton and rigging process methods   //
//==========================================*/
private:
	void ExtractBoneWeights(std::vector<AssetFmt::RawSkinnedVertex>& vertices, aiMesh* mesh);
	void CollectBoneOffsets(const aiScene* scene);
	void FinalizeBoneIDs();

	int32 m_boneCounter{ 0 };
	std::unordered_map<std::string, glm::mat4> m_boneOffsets;
	std::unordered_map<std::string, int32> m_boneNameToIdMap;
};