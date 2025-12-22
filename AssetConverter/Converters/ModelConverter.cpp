#include "pch.h"
#include "ModelConverter.h"

/*==========================================//
//  [Public Entry] 변환 시작 및 상태 초기화   //
//==========================================*/
bool ModelConverter::Convert(const std::string& inputPath, 
                             const std::string& outputPath, bool extractORM)
{
    // [중요] 상태 초기화 (State Clear)
    m_rawModel = AssetFmt::RawModel();
    m_boneNameToIdMap.clear();
    m_boneCounter = 0;
    m_extractORM = extractORM;
    m_inputPath.clear();
    m_outputPath.clear();
    m_modelDirectory.clear();
    
    // 로그 출력 및 입출력 경로 캐싱
    LOG_INFO(" [ModelConverter] Start Conversion");
    LOG_INFO(" - Input:  {}", inputPath);
    LOG_INFO(" - Output: {}", outputPath);
    m_inputPath = inputPath;
    m_outputPath = outputPath;

    // 실제 로직 실행
    bool result = RunConversion();

    if (result) LOG_INFO(" [Success] Conversion Completed.");
    else LOG_ERROR(" [Failed] Conversion Aborted.");

    return result;
}

/*============================//
//   main coversion methods   //
//============================*/
bool ModelConverter::RunConversion()
{
    // 0. 초기화
    m_rawModel = AssetFmt::RawModel(); // 모델 데이터 초기화
    m_boneNameToIdMap.clear();
    m_boneCounter = 0;

    // 0-1. 모델 이름과 상위 경로를 추출
    const fs::path outPath(m_outputPath);
    m_modelDirectory = outPath.parent_path().string();
    m_modelName = outPath.stem().string();
    if (m_modelName.empty()) m_modelName = "unnamed_model";

    // 1. Assimp 임포터 실행
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile
    (
        m_inputPath,
        aiProcess_Triangulate      |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_LimitBoneWeights |
        aiProcess_FlipUVs
    );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("Assimp ReadFile failed: {}", importer.GetErrorString());
        return false;
    }

    // 2. 머티리얼 처리
    LOG_INFO("Processing materials... Total: {}", scene->mNumMaterials);
    m_rawModel.materials.reserve(scene->mNumMaterials);
    for (uint32 i = 0; i < scene->mNumMaterials; ++i)
    {
        m_rawModel.materials.push_back(ProcessMaterial(scene->mMaterials[i], i));
    }

    // 3. 노드/메쉬 처리 (재귀 호출 시작)
    LOG_INFO("Processing meshes...");
    ProcessNode(scene->mRootNode, scene);

    // 4. 계층 구조 완성 (Hierarchy Extraction)
    LOG_INFO("Processing node hierarchy...");
    int32 startIndex = 0;
    ProcessHierarchy(scene->mRootNode, -1, startIndex);
    LOG_INFO(" - Total Nodes extracted: {}", m_rawModel.nodes.size());

    // 5. 스켈레톤 존재 여부 최종 확정
    m_rawModel.hasSkeleton = (m_boneCounter > 0);
    if (m_rawModel.hasSkeleton) LOG_INFO(" - Skeleton Detected (Total Bones: {})", m_boneCounter);
    else LOG_INFO(" - Static Mesh Detected (No Skeleton)");

    // 6. 파일 저장 (직렬화)
    LOG_INFO("Writing Binary File...");
    if (!WriteCustomModelFile())
    {
        LOG_ERROR("Failed to write output file: {}", m_outputPath);
        return false;
    }

    return true;
}

bool ModelConverter::WriteCustomModelFile()
{
    LOG_INFO("Attempting to write file to: {}", m_outputPath);
    std::ofstream outFile(m_outputPath, std::ios::binary);
    if (!outFile) return false;

    // [DEBUG] 시작 위치
    long pos = (long)outFile.tellp();
    LOG_WARN(">>> [WRITER] Start Offset: {}", pos);

    // 1. Header
    uint32 magic = 0x4D594D44; uint32 version = 2;
    uint32 matCount = (uint32)m_rawModel.materials.size();
    uint32 meshCount = (uint32)m_rawModel.meshes.size();
    bool hasSkeleton = m_rawModel.hasSkeleton;

    AssetUtils::WriteData(outFile, magic);
    AssetUtils::WriteData(outFile, version);
    AssetUtils::WriteData(outFile, matCount);
    AssetUtils::WriteData(outFile, meshCount);
    AssetUtils::WriteData(outFile, hasSkeleton);
    AssetUtils::WriteData(outFile, m_rawModel.globalAABBMin);
    AssetUtils::WriteData(outFile, m_rawModel.globalAABBMax);
    LOG_WARN(">>> [WRITER] Header: {}", (long)outFile.tellp());

    // 2. Hierarchy
    LOG_WARN(">>> [WRITER] Node Hierarchy: {}", (long)outFile.tellp());
    AssetUtils::WriteRawNodes(outFile, m_rawModel.nodes);

    // 2. Skeleton
    if (hasSkeleton)
    {
        uint32 boneCount = (uint32)m_rawModel.boneOffsetInfos.size();
        AssetUtils::WriteData(outFile, boneCount); // 개수

        for (const auto& info : m_rawModel.boneOffsetInfos)
        {
            AssetUtils::WriteData(outFile, info.id);     // 4 bytes
            AssetUtils::WriteData(outFile, info.offset); // 64 bytes
        }

        uint32 mapCount = (uint32)m_boneNameToIdMap.size();
        AssetUtils::WriteData(outFile, mapCount);

        for (const auto& [name, id] : m_boneNameToIdMap)
        {
            AssetUtils::WriteString(outFile, name);
            AssetUtils::WriteData(outFile, id);
        }
    }

    // 3. Materials
    LOG_WARN(">>> [WRITER] Before Materials: {}", (long)outFile.tellp());
    for (const auto& mat : m_rawModel.materials) AssetUtils::WriteRawMaterial(outFile, mat);

    // 4. Meshes
    LOG_WARN(">>> [WRITER] Before Meshes: {}", (long)outFile.tellp());
    for (const auto& mesh : m_rawModel.meshes) AssetUtils::WriteRawMesh(outFile, mesh);

    LOG_WARN(">>> [WRITER] Final Size: {}", (long)outFile.tellp());
    outFile.close();
    return true;
}

void ModelConverter::CreateORMTextureFromAssimp(aiMaterial* material, AssetFmt::RawMaterial& rawMat, int32 index)
{
    // 0. 옵션이 꺼져있으면 즉시 리턴
    if (!m_extractORM) return;

    // 0. 입력 텍스처 파일명 얻어오기
    std::string aoFile = GetTexturePath(material, aiTextureType_AMBIENT_OCCLUSION);
    std::string roughFile = GetTexturePath(material, aiTextureType_DIFFUSE_ROUGHNESS);
    std::string metalFile = GetTexturePath(material, aiTextureType_METALNESS);
    std::string glossyFile = GetTexturePath(material, aiTextureType_SHININESS);
    if (aoFile.empty() && roughFile.empty() &&
        metalFile.empty() && glossyFile.empty()) return;

    // 1. 절대 경로 조립 
    // INFO : 입력 모델 폴더 기준으로 ao, roughness, metallic 텍스쳐가 같이 있다고 가정
    std::string aoAbs    = ResolveTexturePath(aoFile);
    std::string roughAbs = ResolveTexturePath(roughFile);
    std::string metalAbs = ResolveTexturePath(metalFile);
    std::string glossAbs = ResolveTexturePath(glossyFile);

    // 3. Roughness vs Glossiness 결정 로직
    std::string finalRoughPath = roughAbs;
    bool invertRoughness = false;
    if (finalRoughPath.empty() && !glossAbs.empty())
    {
        finalRoughPath = glossAbs;
        invertRoughness = true; // ORM Packer에게 "이거 뒤집어서 써라"라고 지시
        LOG_INFO("  - Roughness map missing. Using Glossiness map instead (Inverted).");
    }

    // 4. ORM 출력 파일명 결정 및 저장 절대 경로 조립
    // INFO : 출력 파일 포멧은 {ModelName}_{Index}_ORM.png
    // INFO : 출력 경로는 .mymodel이 출력되는 경로와 동일
    fs::path outPath(m_outputPath);
    std::string ormFileName = fmt::format("{}_{}_ORM.png", m_modelName, index);
    fs::path savePath = outPath.parent_path() / ormFileName;
    LOG_INFO("[ORM Packer] Baking to: {}", savePath.string());

    // 5. 변환 실행 (마지막 인자로 반전 여부 전달)
    if (!CONV_ORM.Convert(aoAbs, finalRoughPath, metalAbs, savePath.string(), invertRoughness))
    {
        LOG_ERROR("[ORM Packer] Failed to pack ORM texture.");
        return;
    }

    // 4) RawMaterial에는 "파일명만" 저장 (엔진 로더 정책과 일치)
    rawMat.textures.push_back({ ormFileName, AssetFmt::RawTextureType::ORM });
}

/*====================================//
//   default assimp process methods   //
//====================================*/
void ModelConverter::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32 i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (uint32 i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void ModelConverter::ProcessHierarchy(aiNode* node, int32 parentIndex, int32& currentIndex)
{
    AssetFmt::RawNode rawNode;

    // 1. 기본 정보 복사
    rawNode.name = node->mName.C_Str();
    rawNode.parentIndex = parentIndex;

    // 2. 행렬 변환 (Assimp -> GLM)
    rawNode.localTransform = Utils::ConvertToGLMMat4(node->mTransformation);

    // 3. 리스트에 추가 (현재 인덱스 = currentIndex)
    int32 myIndex = currentIndex;
    m_rawModel.nodes.push_back(rawNode);

    // 4. 인덱스 증가
    currentIndex++;

    // 5. 자식 노드 순회 (재귀 호출)
    for (uint32 i = 0; i < node->mNumChildren; i++)
        ProcessHierarchy(node->mChildren[i], myIndex, currentIndex);
}

void ModelConverter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    AssetFmt::RawMesh rawMesh;
    rawMesh.name = mesh->mName.C_Str();
    rawMesh.materialIndex = mesh->mMaterialIndex;

    // [LOG] 메쉬 기본 정보 출력
    LOG_INFO("Processing Mesh: '{}'", rawMesh.name);
    LOG_INFO("  - Material Index: {}", rawMesh.materialIndex);
    LOG_INFO("  - Vertex Count: {}", mesh->mNumVertices);
    LOG_INFO("  - Face Count: {}", mesh->mNumFaces);
    LOG_INFO("  - Bone Count: {}", mesh->mNumBones);
    LOG_INFO("  - Has TextureCoords: {}", mesh->HasTextureCoords(0) ? "Yes" : "No");
    LOG_INFO("  - Has Tangents: {}", mesh->HasTangentsAndBitangents() ? "Yes" : "No");

    // 1. 스킨드(애니메이션) 메쉬인지 판단
    rawMesh.isSkinned = (mesh->mNumBones > 0);
    LOG_INFO("  - Mesh Type: {}", rawMesh.isSkinned ? "Skinned Mesh" : "Static Mesh");

    // 2. 바운딩 박스 초기화 (최대값으로 뒤집어서 시작)
    rawMesh.aabbMin = glm::vec3(FLT_MAX);
    rawMesh.aabbMax = glm::vec3(-FLT_MAX);

    // 2. 타입별 정점 처리 (함수 분리)
    if (rawMesh.isSkinned) ProcessSkinnedMesh(mesh, rawMesh);
    else ProcessStaticMesh(mesh, rawMesh);

    // [LOG] AABB 결과 확인 (너무 크거나 작으면 스케일/단위 문제 의심)
    LOG_INFO("  - AABB Min: ({:.4f}, {:.4f}, {:.4f})", rawMesh.aabbMin.x, rawMesh.aabbMin.y, rawMesh.aabbMin.z);
    LOG_INFO("  - AABB Max: ({:.4f}, {:.4f}, {:.4f})", rawMesh.aabbMax.x, rawMesh.aabbMax.y, rawMesh.aabbMax.z);

    // 4. 인덱스 복사 (공통)
    rawMesh.indices.reserve(mesh->mNumFaces * 3);
    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        rawMesh.indices.push_back(mesh->mFaces[i].mIndices[0]);
        rawMesh.indices.push_back(mesh->mFaces[i].mIndices[1]);
        rawMesh.indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    // 5. 최종 결과 모델에 추가
    m_rawModel.meshes.push_back(std::move(rawMesh));
}

void ModelConverter::ProcessSkinnedMesh(aiMesh* mesh, AssetFmt::RawMesh& rawMesh)
{
    // 1. 정점 데이터 채우기
    rawMesh.skinnedVertices.resize(mesh->mNumVertices);

    for (uint32 i = 0; i < mesh->mNumVertices; i++)
    {
        auto& v = rawMesh.skinnedVertices[i];

        // 위치, 노멀
        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        // UV
        if (mesh->mTextureCoords[0]) v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        else v.texCoord = { 0.0f, 0.0f };

        // Tangent
        if (mesh->mTangents) v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        else v.tangent = { 0.0f, 0.0f, 0.0f };

        // AABB 갱신 (Global & Local)
        rawMesh.aabbMin = Utils::Min(rawMesh.aabbMin, v.position);
        rawMesh.aabbMax = Utils::Max(rawMesh.aabbMax, v.position);
        m_rawModel.globalAABBMin = Utils::Min(m_rawModel.globalAABBMin, v.position);
        m_rawModel.globalAABBMax = Utils::Max(m_rawModel.globalAABBMax, v.position);
    }

    // 2. 뼈 가중치 추출 (이미 별도 함수로 분리되어 있음)
    ExtractBoneWeights(rawMesh.skinnedVertices, mesh);
}

void ModelConverter::ProcessStaticMesh(aiMesh* mesh, AssetFmt::RawMesh& rawMesh)
{
    rawMesh.staticVertices.resize(mesh->mNumVertices);

    for (uint32 i = 0; i < mesh->mNumVertices; i++)
    {
        auto& v = rawMesh.staticVertices[i];

        // 위치, 노멀
        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        // UV
        if (mesh->mTextureCoords[0]) v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        else v.texCoord = { 0.0f, 0.0f };

        // Tangent
        if (mesh->mTangents) v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        else v.tangent = { 0.0f, 0.0f, 0.0f };

        // AABB 갱신 (Global & Local)
        rawMesh.aabbMin = Utils::Min(rawMesh.aabbMin, v.position);
        rawMesh.aabbMax = Utils::Max(rawMesh.aabbMax, v.position);
        m_rawModel.globalAABBMin = Utils::Min(m_rawModel.globalAABBMin, v.position);
        m_rawModel.globalAABBMax = Utils::Max(m_rawModel.globalAABBMax, v.position);
    }
}

AssetFmt::RawMaterial ModelConverter::ProcessMaterial(aiMaterial* material, int32 index)
{
    AssetFmt::RawMaterial rawMat;
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) rawMat.name = name.C_Str();
    else rawMat.name = "DefaultMaterial";

    // 1. 머티리얼 속성 추출
    aiColor4D color;
    float value;

    // 1-1. Albedo (Base Color)
    // GLTF 같은 PBR 포맷은 AI_MATKEY_BASE_COLOR를 쓰고, 레거시는 COLOR_DIFFUSE를 씀. 우선순위 체크.
    if (material->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS) rawMat.albedoFactor = { color.r, color.g, color.b, color.a };
    else if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) rawMat.albedoFactor = { color.r, color.g, color.b, 1.0f };

    // 1-2. Emissive
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) rawMat.emissiveFactor = { color.r, color.g, color.b };
    if (material->Get(AI_MATKEY_EMISSIVE_INTENSITY, value) == AI_SUCCESS) rawMat.emissiveStrength = value;
    else rawMat.emissiveStrength = 1.0f;

    // 1-3. Metallic & Roughness
    if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) rawMat.metallicFactor = value;
    if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) rawMat.roughnessFactor = value;

    // [LOG] 추출된 Factor 값 확인
    LOG_INFO("  [Factors]");
    LOG_INFO("   - Albedo Color: ({:.2f}, {:.2f}, {:.2f}, {:.2f})", rawMat.albedoFactor.r, rawMat.albedoFactor.g, rawMat.albedoFactor.b, rawMat.albedoFactor.a);
    LOG_INFO("   - Metallic: {:.2f}, Roughness: {:.2f}", rawMat.metallicFactor, rawMat.roughnessFactor);

    // 2. 텍스처 추출 (Helper Lambda 활용)

    // Base Color (우선순위 처리)
    std::string baseColorPath = GetTexturePath(material, aiTextureType_BASE_COLOR);
    if (!baseColorPath.empty()) rawMat.textures.push_back({ baseColorPath, AssetFmt::RawTextureType::Albedo });
    else AddTextureToMaterial(rawMat, material, aiTextureType_DIFFUSE, AssetFmt::RawTextureType::Albedo);
    AddTextureToMaterial(rawMat, material, aiTextureType_NORMALS, AssetFmt::RawTextureType::Normal);
    AddTextureToMaterial(rawMat, material, aiTextureType_HEIGHT, AssetFmt::RawTextureType::Height);
    AddTextureToMaterial(rawMat, material, aiTextureType_SPECULAR, AssetFmt::RawTextureType::Specular);
    AddTextureToMaterial(rawMat, material, aiTextureType_EMISSIVE, AssetFmt::RawTextureType::Emissive);
    AddTextureToMaterial(rawMat, material, aiTextureType_AMBIENT_OCCLUSION, AssetFmt::RawTextureType::AmbientOcclusion);
    AddTextureToMaterial(rawMat, material, aiTextureType_DIFFUSE_ROUGHNESS, AssetFmt::RawTextureType::Roughness);
    AddTextureToMaterial(rawMat, material, aiTextureType_METALNESS, AssetFmt::RawTextureType::Metallic);
    AddTextureToMaterial(rawMat, material, aiTextureType_SHININESS, AssetFmt::RawTextureType::Glossiness);

    // 3. 자동 ORM 텍스쳐 생성
    CreateORMTextureFromAssimp(material, rawMat, index);

    // 4. 텍스쳐 매핑 로깅
    LogFinalMappedTextures(material, rawMat);

    return rawMat;
}

/*=========================================//
//   texture helper methods for material   //
//=========================================*/
std::string ModelConverter::ResolveTexturePath(const std::string& relativePath)
{
    if (relativePath.empty()) return "";

    fs::path p(relativePath);
    fs::path inputDir = fs::path(m_inputPath).parent_path();

    // 1. 이미 절대 경로이고 존재하는 경우
    if (p.is_absolute() && fs::exists(p)) return p.string();

    // 2. 입력 파일과 같은 폴더에 있는 경우
    if (fs::exists(inputDir / p)) return (inputDir / p).string();

    // 3. 파일명만 떼서 같은 폴더에 있는지 확인 (경로가 꼬였을 때 대비)
    if (fs::exists(inputDir / p.filename())) return (inputDir / p.filename()).string();

    return ""; // 못 찾음
}

void ModelConverter::AddTextureToMaterial(AssetFmt::RawMaterial& rawMat, aiMaterial* aiMat, aiTextureType aiType, AssetFmt::RawTextureType rawType)
{
    std::string path = GetTexturePath(aiMat, aiType);
    if (!path.empty())
    {
        AssetFmt::RawTexture tex;
        tex.fileName = path;
        tex.type = rawType;
        rawMat.textures.push_back(tex);
    }
}

std::string ModelConverter::GetTexturePath(aiMaterial* material, aiTextureType type)
{
    if (material->GetTextureCount(type) <= 0) return "";

    aiString filepath;
    if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) return "";

    // 파일 이름만 추출 (경로 정규화)
    std::filesystem::path fullPath(filepath.C_Str());
    std::string filename = fullPath.filename().string();
    if (filename.empty()) return "";

    // [설계 결정] 리턴값은 "파일 이름"만.
    // INFO : 반드시 .mymodel과 이를 이루는 모든 텍스쳐는 같은 경로에 있게 된다.
    return filename;
}

void ModelConverter::LogFinalMappedTextures(aiMaterial* material, const AssetFmt::RawMaterial& rawMat)
{
    LOG_INFO("  [Textures] Count: {}", rawMat.textures.size());
    for (const auto& tex : rawMat.textures)
    {
        std::string typeStr;
        switch (tex.type)
        {
        case AssetFmt::RawTextureType::Albedo: typeStr = "Albedo"; break;
        case AssetFmt::RawTextureType::Normal: typeStr = "Normal"; break;
        case AssetFmt::RawTextureType::Emissive: typeStr = "Emissive"; break;
        case AssetFmt::RawTextureType::Specular: typeStr = "Specular"; break;
        case AssetFmt::RawTextureType::Height:   typeStr = "Height"; break;
        case AssetFmt::RawTextureType::Glossiness: typeStr = "Glossiness"; break;
        case AssetFmt::RawTextureType::Roughness: typeStr = "Roughness"; break;
        case AssetFmt::RawTextureType::Metallic: typeStr = "Metallic"; break;
        case AssetFmt::RawTextureType::AmbientOcclusion: typeStr = "AO"; break;
        case AssetFmt::RawTextureType::ORM: typeStr = "ORM (Packed)"; break;
        default: typeStr = "Unknown"; break;
        }
        // 정렬을 맞춰서 보기 편하게 출력
        LOG_INFO("   - {:<12} : {}", typeStr, tex.fileName);
    }

    // 검사할 텍스처 타입과 이름을 매핑
    const struct { aiTextureType type; const char* name; } checkList[] = 
    {
        { aiTextureType_DIFFUSE, "DIFFUSE (Legacy)" },
        { aiTextureType_BASE_COLOR, "BASE_COLOR (PBR)" },
        { aiTextureType_NORMALS, "NORMALS" },
        { aiTextureType_HEIGHT, "HEIGHT (Bump)" },
        { aiTextureType_SPECULAR, "SPECULAR" },
        { aiTextureType_EMISSIVE, "EMISSIVE" },
        { aiTextureType_UNKNOWN, "UNKNOWN" },
        { aiTextureType_SHININESS, "GLOSSINESS" },
        { aiTextureType_METALNESS, "METALNESS" },
        { aiTextureType_DIFFUSE_ROUGHNESS, "ROUGHNESS" },
        { aiTextureType_AMBIENT_OCCLUSION, "AO" },
    };

    aiString debugPath;
    for (const auto& check : checkList)
    {
        // 각 타입의 0번 인덱스(첫 번째 텍스처)만 확인
        if (material->GetTexture(check.type, 0, &debugPath) == AI_SUCCESS)
            LOG_TRACE("   [Assimp Debug] Found {:<15} : {}", check.name, debugPath.C_Str());
    }
}

/*==============================//
//   skeleton process methods   //
//==============================*/
void ModelConverter::ExtractBoneWeights(std::vector<AssetFmt::RawSkinnedVertex>& vertices, aiMesh* mesh)
{
    // 메쉬가 가진 모든 뼈를 순회
    for (uint32 i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        int32 boneID = -1;
        std::string boneName = bone->mName.C_Str();

        // 1. 뼈 등록 단계 (Global Bone Registry)
        if (m_boneNameToIdMap.find(boneName) == m_boneNameToIdMap.end())
        {
            AssetFmt::RawBoneInfo newBoneInfo;
            newBoneInfo.id = m_boneCounter;
            newBoneInfo.offset = Utils::ConvertToGLMMat4(bone->mOffsetMatrix);

            m_rawModel.boneOffsetInfos.push_back(newBoneInfo);

            // 맵에 등록하고 카운터 증가
            boneID = m_boneCounter;
            m_boneNameToIdMap[boneName] = boneID;
            m_boneCounter++;
        }
        else
        {
            // [이미 등록된 뼈] ID만 가져옴
            boneID = m_boneNameToIdMap[boneName];
        }

        // 2. 가중치 주입 단계 (Vertex Weight Assignment)
        // 이 뼈가 영향을 주는 모든 정점을 순회
        for (uint32 j = 0; j < bone->mNumWeights; ++j)
        {
            const auto& weightData = bone->mWeights[j];
            uint32 vertexId = weightData.mVertexId;
            float weight = weightData.mWeight;

            // 안전 장치: 정점 인덱스가 범위 밖이면 무시
            if (vertexId >= vertices.size()) continue;

            // 해당 정점의 빈 슬롯(-1)을 찾아서 채움 (최대 4개)
            auto& v = vertices[vertexId];
            for (int k = 0; k < MAX_BONE_INFLUENCE; ++k)
            {
                if (v.boneIDs[k] < 0) // -1이면 비어있다는 뜻
                {
                    v.boneIDs[k] = boneID;
                    v.weights[k] = weight;
                    break; // 채웠으면 다음 정점으로
                }
            }
        }
    }
}