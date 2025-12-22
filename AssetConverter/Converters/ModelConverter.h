#pragma once
#include "AssetTypes/AssetFormat.h"

class ModelConverter
{
	DECLARE_SINGLE(ModelConverter)

public:
	bool Convert(const std::string& inputPath, 
				 const std::string& outputPath, bool extractORM);

private:
	bool RunConversion();

	// Assimp 처리 메서드
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	AssetFmt::RawMaterial ProcessMaterial(aiMaterial* material, int32 index);

	// 뼈 가중치 추출 (SkinnedVertex일 때만 사용)
	void ExtractBoneWeights
	(
		std::vector<AssetFmt::RawSkinnedVertex>& vertices, 
		aiMesh* mesh
	);

	// 텍스쳐 경로 및 파일 쓰기
	std::string GetTexturePath(aiMaterial* material, aiTextureType type);
	void CreateORMTextureFromAssimp
	(
		aiMaterial* material, 
		AssetFmt::RawMaterial& rawMat, int32 index
	);
	bool WriteCustomModelFile();

private:
	// 모델 데이터 구조체
	AssetFmt::RawModel m_rawModel;

	// 경로 문자열 멤버
	std::string m_inputPath;
	std::string m_outputPath;
	std::string m_modelDirectory;
	std::string m_modelName;

	// 옵션 캐싱
	bool m_extractORM		{ false };

	// 뼈 ID 할당용 카운터 (RawModel.boneOffsetInfos의 인덱스와 동일)
	int32 m_boneCounter		{ 0 };

	// 뼈 이름 중복 등록 방지 및 ID 검색용 맵
	std::unordered_map<std::string, int32> m_boneNameToIdMap;
};