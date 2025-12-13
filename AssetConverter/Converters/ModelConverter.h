#pragma once
#include "AssetTypes/RawModel.h"

class ModelConverter
{
	DECLARE_SINGLE(ModelConverter)

public:
	bool Convert(const std::string& inputPath, const std::string& outputPath);

private:
	bool RunConversion(const std::string& inputPath, const std::string& outputPath);

	// Assimp 처리 메서드
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	RawMaterial ProcessMaterial(aiMaterial* material);

	// 뼈 가중치 추출 (SkinnedVertex일 때만 사용)
	void ExtractBoneWeights(std::vector<RawSkinnedVertex>& vertices, aiMesh* mesh);

	// 텍스쳐 경로 및 파일 쓰기
	std::string GetTexturePath(aiMaterial* material, aiTextureType type);
	bool WriteCustomModelFile(const std::string& outputPath);

private:
	// 기존의 잡다한 벡터들을 이거 하나로 통합!
	RawModel m_rawModel;

	// 경로 계산용 임시 변수
	std::string m_modelDirectory;

	// 뼈 ID 할당용 카운터 (RawModel.boneOffsetInfos의 인덱스와 동일)
	int32 m_boneCounter = 0;

	// 뼈 이름 중복 등록 방지 및 ID 검색용 맵
	std::unordered_map<std::string, int32> m_boneNameToIdMap;
};