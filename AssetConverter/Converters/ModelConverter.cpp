#include "../pch.h"
#include "ModelConverter.h"

/*=========================//
//  converter entry point  //
//=========================*/
bool ModelConverter::Convert(const std::string& inputPath, 
							 const std::string& outputPath)
{
	ModelConverter converter;
	SPDLOG_INFO("Creating ModelConverter instance and starting conversion...");
	return converter.RunConversion(inputPath, outputPath);
}

/*===========================//
//  main conversion methods  //
//===========================*/
bool ModelConverter::RunConversion(const std::string& inputPath, const std::string& outputPath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(inputPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		SPDLOG_ERROR("Assimp load failed: {}", importer.GetErrorString());
		return false;
	}

	m_modelDirectory = std::filesystem::path(inputPath).parent_path().string();

	// 1-1. 머티리얼 처리
	SPDLOG_INFO("Processing materials... Total: {}", scene->mNumMaterials);
	for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		m_materials.push_back(ProcessMaterial(scene->mMaterials[i]));

	// 1-2. 노드/메쉬 처리
	SPDLOG_INFO("Starting node and mesh processing...");
	ProcessNode(scene->mRootNode, scene);
	SPDLOG_INFO("Total {} meshes processed.", m_meshes.size());
	if (m_hasSkeleton)
		SPDLOG_INFO("Skeleton found. Total {} bones.", m_boneInfoMap.size());

	// 2. 커스텀 바이너리 파일 쓰기
	SPDLOG_INFO("Starting to write custom model file: {}", outputPath);
	if (!WriteCustomModelFile(outputPath))
	{
		SPDLOG_ERROR("Failed to write custom model file: {}", outputPath);
		return false;
	}

	SPDLOG_INFO("Model conversion successful!");
	return true;
}

/*=========================//
//   extract assimp data   //
//=========================*/
void ModelConverter::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		auto meshIndex = node->mMeshes[i];
		auto mesh = scene->mMeshes[meshIndex];
		ProcessMesh(mesh, scene);
	}

	for (uint32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

void ModelConverter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	TempMesh tempMesh;
	std::vector<SkinnedVertex> vertices;

	// 1. 정점 데이터 채우기
	vertices.resize(mesh->mNumVertices);
	for (uint32 i = 0; i < mesh->mNumVertices; i++)
	{
		auto& v = vertices[i];
		SetVertexBoneDataToDefault(v);
		v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		if (mesh->mTextureCoords[0])
			v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		else
			v.texCoord = { 0.0f, 0.0f };

		if (mesh->mTangents)
		{
			v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		}
		else
		{
			v.tangent = { 0.0f, 0.0f, 0.0f };
		}
	}

	// 2. 인덱스 데이터 채우기
	std::vector<uint32> indices;
	indices.resize(mesh->mNumFaces * 3);
	for (uint32 i = 0; i < mesh->mNumFaces; i++)
	{
		indices[3 * i] = mesh->mFaces[i].mIndices[0];
		indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
		indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
	}

	// 3. 뼈 가중치 추출
	if (mesh->mNumBones > 0)
	{
		m_hasSkeleton = true; // 스켈레톤이 있다고 표시
		ExtractBoneWeightForVertices(vertices, mesh);
	}

	// 4. TempMesh에 데이터 저장
	tempMesh.vertices = std::move(vertices);
	tempMesh.indices = std::move(indices);
	tempMesh.materialIndex = mesh->mMaterialIndex;

	// 5. 멤버 변수 m_meshes에 추가
	m_meshes.push_back(std::move(tempMesh));
}

TempMaterial ModelConverter::ProcessMaterial(aiMaterial* material)
{
	TempMaterial tempMat;
	tempMat.diffuseMapPath = GetTexturePath(material, aiTextureType_DIFFUSE);
	tempMat.specularMapPath = GetTexturePath(material, aiTextureType_SPECULAR);
	tempMat.emissionMapPath = GetTexturePath(material, aiTextureType_EMISSIVE);
	tempMat.normalMapPath = GetTexturePath(material, aiTextureType_NORMALS);
	return tempMat;
}

std::string ModelConverter::GetTexturePath(aiMaterial* material, aiTextureType type)
{
	if (material->GetTextureCount(type) <= 0) return "";

	aiString filepath;
	if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) return "";

	// 텍스처 파일 이름만 추출
	std::string filenameOnly = std::filesystem::path(filepath.C_Str()).filename().string();
	if (filenameOnly.empty()) return "";

	// 원본 모델 디렉터리와 파일 이름을 조합 (상대 경로 유지)
	return (std::filesystem::path(m_modelDirectory) / filenameOnly).string();
}

void ModelConverter::ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, aiMesh* mesh)
{
	for (uint32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

		if (m_boneInfoMap.find(boneName) == m_boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_boneCounter;
			newBoneInfo.offset = Utils::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			m_boneInfoMap[boneName] = newBoneInfo;
			boneID = m_boneCounter;
			m_boneCounter++;
		}
		else
		{
			boneID = m_boneInfoMap[boneName].id;
		}

		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

/*===========================//
//   wirte custom binaries   //
//===========================*/
bool ModelConverter::WriteCustomModelFile(const std::string& outputPath)
{
	std::ofstream outFile(outputPath, std::ios::binary);
	if (!outFile)
	{
		SPDLOG_ERROR("Failed to open output file: {}", outputPath);
		return false;
	}

	// --- 1. 파일 헤더 쓰기 ---
	uint32 magic = 0x4D594D44; // 'MYMD' (My Model)
	uint32 version = 1;
	uint32 materialCount = (uint32)m_materials.size();
	uint32 meshCount = (uint32)m_meshes.size();

	ConverterUtils::WriteData(outFile, magic);
	ConverterUtils::WriteData(outFile, version);
	ConverterUtils::WriteData(outFile, materialCount);
	ConverterUtils::WriteData(outFile, meshCount);
	ConverterUtils::WriteData(outFile, m_hasSkeleton);

	// --- 2. 스켈레톤 블록 쓰기 ---
	if (m_hasSkeleton)
	{
		uint32 boneCount = (uint32)m_boneInfoMap.size();
		ConverterUtils::WriteData(outFile, boneCount);

		for (auto const& [name, boneInfo] : m_boneInfoMap)
		{
			ConverterUtils::WriteString(outFile, name); // 뼈 이름
			ConverterUtils::WriteData(outFile, boneInfo); // BoneInfo struct (id, offset)
		}
	}

	// --- 3. 머티리얼 블록 쓰기 ---
	for (const auto& material : m_materials)
	{
		ConverterUtils::WriteString(outFile, material.diffuseMapPath);
		ConverterUtils::WriteString(outFile, material.specularMapPath);
		ConverterUtils::WriteString(outFile, material.emissionMapPath);
		ConverterUtils::WriteString(outFile, material.normalMapPath);
	}

	// --- 4. 메쉬 블록 쓰기 ---
	for (const auto& mesh : m_meshes)
	{
		// 메쉬 헤더
		ConverterUtils::WriteData(outFile, mesh.materialIndex);
		uint32 vertexCount = (uint32)mesh.vertices.size();
		uint32 indexCount = (uint32)mesh.indices.size();
		ConverterUtils::WriteData(outFile, vertexCount);
		ConverterUtils::WriteData(outFile, indexCount);

		// 메쉬 데이터 (데이터 덩어리 통째로 쓰기)
		outFile.write(reinterpret_cast<const char*>(mesh.vertices.data()),
			sizeof(SkinnedVertex) * vertexCount);
		outFile.write(reinterpret_cast<const char*>(mesh.indices.data()),
			sizeof(uint32) * indexCount);
	}

	outFile.close();
	SPDLOG_INFO("Binary file write complete.");
	return true;
}

/*==================//
//   bone helpers   //
//==================*/
void ModelConverter::SetVertexBoneDataToDefault(SkinnedVertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void ModelConverter::SetVertexBoneData(SkinnedVertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.boneIDs[i] < 0)
		{
			vertex.weights[i] = weight;
			vertex.boneIDs[i] = boneID;
			break;
		}
	}
}