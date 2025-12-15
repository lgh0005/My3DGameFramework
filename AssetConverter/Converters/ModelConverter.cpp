#include "pch.h"
#include "ModelConverter.h"

/*==========================================//
//  [Public Entry] 변환 시작 및 상태 초기화   //
//==========================================*/
bool ModelConverter::Convert(const std::string& inputPath, 
                             const std::string& outputPath, bool extractORM)
{
    // [중요] 상태 초기화 (State Clear)
    // 싱글톤은 데이터가 유지되므로 매번 실행 시 리셋해야 합니다.
    m_rawModel = RawModel(); // 새 객체로 덮어씌워 초기화
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

/*==================//
//  Internal Logic  //
//==================*/
// INFO : Assimp 로드 -> 변환(Materials/Nodes) -> 저장
bool ModelConverter::RunConversion()
{
    // 0. 초기화
    m_rawModel = RawModel(); // 모델 데이터 초기화
    m_boneNameToIdMap.clear();
    m_boneCounter = 0;

    // 0-1. 모델 이름과 상위 경로를 추출
    const fs::path outPath(m_outputPath);
    m_modelDirectory = outPath.parent_path().string();
    m_modelName = outPath.stem().string();
    if (m_modelName.empty()) m_modelName = "unnamed_model";

    // 1. Assimp 임포터 실행
    // [옵션 설명]
    // - Triangulate: 모든 면을 삼각형으로
    // - CalcTangentSpace: 노멀 매핑을 위해 탄젠트 계산
    // - GenSmoothNormals: 노멀이 없으면 부드럽게 생성
    // - LimitBoneWeights: 하나의 정점에 최대 4개의 뼈만 영향 (우리 엔진 규격)
    // - ConvertToLeftHanded: DirectX 등을 쓸 때 필요하지만, OpenGL은 보통 생략 가능 (상황에 따라)
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
        return false;

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

    // 4. 스켈레톤 존재 여부 최종 확정
    m_rawModel.hasSkeleton = (m_boneCounter > 0);
    if (m_rawModel.hasSkeleton) LOG_INFO(" - Skeleton Detected (Total Bones: {})", m_boneCounter);
    else LOG_INFO(" - Static Mesh Detected (No Skeleton)");

    // 5. 파일 저장 (직렬화)
    LOG_INFO("Writing Binary File...");
    if (!WriteCustomModelFile())
    {
        LOG_ERROR("Failed to write output file: {}", m_outputPath);
        return false;
    }

    return true;
}

// INFO : Header -> Skeleton -> Materials -> Meshes 순서
bool ModelConverter::WriteCustomModelFile()
{
    LOG_INFO("Attempting to write file to: {}", m_outputPath);

    std::ofstream outFile(m_outputPath, std::ios::binary);
    if (!outFile)
    {
        // errno를 통해 구체적인 시스템 에러 메시지 확인
        LOG_ERROR("Failed to open std::ofstream!");
        LOG_ERROR("Error Code (errno): {}", errno);
        LOG_ERROR("Error Message: {}", std::strerror(errno));
        return false;
    }
    LOG_INFO("File opened successfully.");

    // 1. 헤더 (Header)
    uint32 magic = 0x4D594D44; // 'MYMD'
    uint32 version = 2;        // 버전 2로 업데이트
    uint32 matCount = (uint32)m_rawModel.materials.size();
    uint32 meshCount = (uint32)m_rawModel.meshes.size();
    bool hasSkeleton = m_rawModel.hasSkeleton;
    LOG_INFO("Writing Header - Mats: {}, Meshes: {}, Skel: {}", matCount, meshCount, hasSkeleton);

    Utils::WriteData(outFile, magic);
    Utils::WriteData(outFile, version);
    Utils::WriteData(outFile, matCount);
    Utils::WriteData(outFile, meshCount);
    Utils::WriteData(outFile, hasSkeleton);

    // 전역 AABB (엔진의 초기 컬링을 위해 헤더에 포함)
    Utils::WriteData(outFile, m_rawModel.globalAABBMin);
    Utils::WriteData(outFile, m_rawModel.globalAABBMax);

    if (outFile.fail())
    {
        LOG_ERROR("File write failed after Header.");
        return false;
    }

    // 2. 스켈레톤 정보 (Skeleton Info)
    // 엔진은 (뼈이름 -> 뼈정보) 순서로 읽어서 맵핑함
    // TODO : 이후에 이 부분은 Avatar나 Skeleton과 같은 곳에서 읽을 수 있는
    // 데이터로 뺄 수 있어야 함
    if (hasSkeleton)
    {
        uint32 boneCount = (uint32)m_rawModel.boneOffsetInfos.size();
        LOG_INFO("Writing Skeleton Info ({} bones)", boneCount);
        Utils::WriteData(outFile, boneCount);

        // 맵을 순회하며 저장 (이름과 ID를 매칭시키기 위해)
        for (const auto& [name, id] : m_boneNameToIdMap)
        {
            // 1. 뼈 이름 쓰기
            Utils::WriteString(outFile, name);

            // 2. 뼈 정보(ID, Offset) 쓰기
            // ID로 벡터에서 정보 가져오기
            const RawBoneInfo& info = m_rawModel.boneOffsetInfos[id];
            Utils::WriteData(outFile, info);
        }
    }
    if (outFile.fail())
    {
        LOG_ERROR("File write failed after Skeleton.");
        return false;
    }

    // 3. 머티리얼 (Materials)
    LOG_INFO("Writing Materials...");
    for (const auto& mat : m_rawModel.materials)
    {
        // 텍스처 경로들 쓰기 (엔진 로더와 일치시켜야 함!)
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Albedo));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Normal));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Emissive));

        // [PBR / ORM]
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::ORM));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::AmbientOcclusion));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Roughness));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Metallic));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Glossiness));

        // [Legacy]
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Specular));
        Utils::WriteString(outFile, mat.GetTexturePath(RawTextureType::Height));

        // [New] PBR Factors (텍스처 없을 때를 대비한 수치값)
        Utils::WriteData(outFile, mat.albedoFactor);
        Utils::WriteData(outFile, mat.metallicFactor);
        Utils::WriteData(outFile, mat.roughnessFactor);
    }
    if (outFile.fail())
    {
        LOG_ERROR("File write failed after Materials.");
        return false;
    }

    // 4. 메쉬 (Meshes)
    LOG_INFO("Writing Meshes...");
    for (const auto& mesh : m_rawModel.meshes)
    {
        // 메쉬 헤더
        Utils::WriteData(outFile, mesh.materialIndex);
        Utils::WriteData(outFile, mesh.isSkinned); // [V2 추가] 스킨드 여부 플래그

        // AABB (로컬)
        Utils::WriteData(outFile, mesh.aabbMin);
        Utils::WriteData(outFile, mesh.aabbMax);

        // 버텍스/인덱스 개수
        uint32 vCount = mesh.isSkinned ? (uint32)mesh.skinnedVertices.size() : (uint32)mesh.staticVertices.size();
        uint32 iCount = (uint32)mesh.indices.size();
        Utils::WriteData(outFile, vCount);
        Utils::WriteData(outFile, iCount);

        // 버텍스 데이터 (분기 처리)
        if (mesh.isSkinned)
        {
            // RawSkinnedVertex 저장
            outFile.write(reinterpret_cast<const char*>(mesh.skinnedVertices.data()), sizeof(RawSkinnedVertex) * vCount);
        }
        else
        {
            // RawStaticVertex 저장
            outFile.write(reinterpret_cast<const char*>(mesh.staticVertices.data()), sizeof(RawStaticVertex) * vCount);
        }

        // 인덱스 데이터
        outFile.write(reinterpret_cast<const char*>(mesh.indices.data()), sizeof(uint32) * iCount);
    }
    if (outFile.fail())
    {
        LOG_ERROR("File write failed after Meshes.");
        return false;
    }

    LOG_INFO("File write completed successfully.");
    outFile.close();
    return true;
}

/*==================================//
//  [Processing] 모델의 데이터 추출  //
//==================================*/
void ModelConverter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    RawMesh rawMesh;
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

    /*================================================//
    //  스킨드 메쉬 (뼈 있음) -> RawSkinnedVertex 사용  //
    //================================================*/
    if (rawMesh.isSkinned)
    {
        // 3-1. 정점 데이터 채우기
        rawMesh.skinnedVertices.resize(mesh->mNumVertices);
        for (uint32 i = 0; i < mesh->mNumVertices; i++)
        {
            auto& v = rawMesh.skinnedVertices[i];

            // 위치, 노멀, UV, 탄젠트 복사
            v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            // assimp에서 uv좌표와 tangent를 로드
            if (mesh->mTextureCoords[0]) v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            else v.texCoord = { 0.0f, 0.0f };
            if (mesh->mTangents) v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            else  v.tangent = { 0.0f, 0.0f, 0.0f };

            // AABB 갱신 (Global & Local 동시 갱신)
            rawMesh.aabbMin = Utils::Min(rawMesh.aabbMin, v.position);
            rawMesh.aabbMax = Utils::Max(rawMesh.aabbMax, v.position);
            m_rawModel.globalAABBMin = Utils::Min(m_rawModel.globalAABBMin, v.position);
            m_rawModel.globalAABBMax = Utils::Max(m_rawModel.globalAABBMax, v.position);
        }

        // 3-2. 뼈 가중치 추출 (별도 함수로 분리)
        ExtractBoneWeights(rawMesh.skinnedVertices, mesh);
    }
    /*================================================//
    //  스태틱 메쉬 (뼈 없음) -> RawStaticVertex 사용   //
    //================================================*/
    else
    {
        rawMesh.staticVertices.resize(mesh->mNumVertices);
        for (uint32 i = 0; i < mesh->mNumVertices; i++)
        {
            auto& v = rawMesh.staticVertices[i];

            // (BoneID, Weight 제외하고 똑같이 복사)
            v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            // assimp에서 uv좌표와 tangent를 로드
            if (mesh->mTextureCoords[0]) v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            else v.texCoord = { 0.0f, 0.0f };
            if (mesh->mTangents) v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            else v.tangent = { 0.0f, 0.0f, 0.0f };

            // AABB 갱신 (Global & Local 동시 갱신)
            rawMesh.aabbMin = Utils::Min(rawMesh.aabbMin, v.position);
            rawMesh.aabbMax = Utils::Max(rawMesh.aabbMax, v.position);
            m_rawModel.globalAABBMin = Utils::Min(m_rawModel.globalAABBMin, v.position);
            m_rawModel.globalAABBMax = Utils::Max(m_rawModel.globalAABBMax, v.position);
        }
    }

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

RawMaterial ModelConverter::ProcessMaterial(aiMaterial* material, int32 index)
{
    RawMaterial rawMat;
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) rawMat.name = name.C_Str();
    else rawMat.name = "DefaultMaterial";

    // 1. 머티리얼 속성 추출
    aiColor4D color;
    float value;

    // 1-1. Albedo (Base Color)
    // GLTF 같은 PBR 포맷은 AI_MATKEY_BASE_COLOR를 쓰고, 레거시는 COLOR_DIFFUSE를 씀. 우선순위 체크.
    if (material->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS)
    {
        rawMat.albedoFactor = { color.r, color.g, color.b, color.a };
    }
    else if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        rawMat.albedoFactor = { color.r, color.g, color.b, 1.0f }; // Diffuse는 보통 Alpha가 없으므로 1.0
    }

    // 1-2. Emissive
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
    {
        rawMat.emissiveFactor = { color.r, color.g, color.b };
    }

    // 1-3. Metallic & Roughness
    // GLTF 표준 PBR 속성 확인
    if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
        rawMat.metallicFactor = value;

    if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
        rawMat.roughnessFactor = value;

    // [LOG] 추출된 Factor 값 확인
    LOG_INFO("  [Factors]");
    LOG_INFO("   - Albedo Color: ({:.2f}, {:.2f}, {:.2f}, {:.2f})", rawMat.albedoFactor.r, rawMat.albedoFactor.g, rawMat.albedoFactor.b, rawMat.albedoFactor.a);
    LOG_INFO("   - Metallic: {:.2f}, Roughness: {:.2f}", rawMat.metallicFactor, rawMat.roughnessFactor);

    // 2. 텍스처 추출 (Helper Lambda 활용)
    auto AddTexture = [&](aiTextureType aiType, RawTextureType rawType)
    {
        std::string path = GetTexturePath(material, aiType);
        if (!path.empty())
        {
            RawTexture tex;
            tex.fileName = path;
            tex.type = rawType;
            rawMat.textures.push_back(tex);
        }
    };

    // Assimp 타입 <-> 우리 타입 매핑
    // PBR Albedo는 BASE_COLOR 혹은 DIFFUSE
    std::string baseColorPath = GetTexturePath(material, aiTextureType_BASE_COLOR);
    if (!baseColorPath.empty()) rawMat.textures.push_back({ baseColorPath, RawTextureType::Albedo });
    else AddTexture(aiTextureType_DIFFUSE, RawTextureType::Albedo);
    AddTexture(aiTextureType_NORMALS, RawTextureType::Normal);
    AddTexture(aiTextureType_HEIGHT, RawTextureType::Height);
    AddTexture(aiTextureType_SPECULAR, RawTextureType::Specular);
    AddTexture(aiTextureType_EMISSIVE, RawTextureType::Emissive);
    AddTexture(aiTextureType_AMBIENT_OCCLUSION, RawTextureType::AmbientOcclusion);
    AddTexture(aiTextureType_DIFFUSE_ROUGHNESS, RawTextureType::Roughness);
    AddTexture(aiTextureType_METALNESS, RawTextureType::Metallic);
    AddTexture(aiTextureType_SHININESS, RawTextureType::Glossiness);

    // 3. 자동 ORM 텍스쳐 생성
    CreateORMTextureFromAssimp(material, rawMat, index);

    // [LOG] 최종 매핑된 텍스처 리스트 확인
    LOG_INFO("  [Textures] Count: {}", rawMat.textures.size());
    for (const auto& tex : rawMat.textures)
    {
        std::string typeStr;
        switch (tex.type)
        {
        case RawTextureType::Albedo: typeStr = "Albedo"; break;
        case RawTextureType::Normal: typeStr = "Normal"; break;
        case RawTextureType::Emissive: typeStr = "Emissive"; break;
        case RawTextureType::Specular: typeStr = "Specular"; break;
        case RawTextureType::Height:   typeStr = "Height"; break;
        case RawTextureType::Glossiness: typeStr = "Glossiness"; break;
        case RawTextureType::Roughness: typeStr = "Roughness"; break;
        case RawTextureType::Metallic: typeStr = "Metallic"; break;
        case RawTextureType::AmbientOcclusion: typeStr = "AO"; break;
        case RawTextureType::ORM: typeStr = "ORM (Packed)"; break;
        default: typeStr = "Unknown"; break;
        }
        // 정렬을 맞춰서 보기 편하게 출력
        LOG_INFO("   - {:<12} : {}", typeStr, tex.fileName);
    }

    // 파일 포맷마다 매핑되는 키가 제각각이라, 문제가 생기면 이 로그를 보고 추적합니다.
    LOG_TRACE("  [Assimp Raw Debug] Checking all texture slots...");

    // 검사할 텍스처 타입과 이름을 매핑
    const struct { aiTextureType type; const char* name; } checkList[] = {
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
        {
            LOG_TRACE("   [Assimp Debug] Found {:<15} : {}", check.name, debugPath.C_Str());
        }
    }

    return rawMat;
}

void ModelConverter::ExtractBoneWeights(std::vector<RawSkinnedVertex>& vertices, aiMesh* mesh)
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
            RawBoneInfo newBoneInfo;
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

std::string ModelConverter::GetTexturePath(aiMaterial* material, aiTextureType type)
{
    if (material->GetTextureCount(type) <= 0) return "";

    aiString filepath;
    if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) return "";

    // 파일 이름만 추출 (경로 정규화)
    std::filesystem::path fullPath(filepath.C_Str());
    std::string filename = fullPath.filename().string();
    if (filename.empty()) return "";

    // [설계 결정] 리턴값은 "파일 이름"만? 아니면 "상대 경로"?
    // 일단 텍스처 파일이 모델과 같은 폴더(혹은 하위)에 있다고 가정하고
    // 나중에 로딩할 때 조합할 수 있도록 파일 이름(혹은 상대경로)을 반환.
    // 여기서는 원본 파일명을 그대로 반환하고, 
    // 실제 파일 복사/변환은 나중에 RunConversion의 후처리 단계에서 진행.
    // INFO : 반드시 .mymodel과 이를 이루는 모든 텍스쳐는 같은 경로에 있게 된다.
    return filename;
}

void ModelConverter::CreateORMTextureFromAssimp(aiMaterial* material, RawMaterial& rawMat, int32 index)
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
    fs::path inputDir = fs::path(m_inputPath).parent_path();
    auto ToAbsInput = [&](const std::string& fileName) -> std::string
    {
        if (fileName.empty()) return {};
        fs::path p(fileName);

        // 절대 경로거나, 상대 경로로 찾았거나, 파일명만으로 찾았거나 (Fallback 로직 포함)
        if (p.is_absolute() && fs::exists(p)) return p.string();
        if (fs::exists(inputDir / p)) return (inputDir / p).string();
        if (fs::exists(inputDir / p.filename())) return (inputDir / p.filename()).string();
        return ""; // 파일 없음
    };
    std::string aoAbs    = ToAbsInput(aoFile);
    std::string roughAbs = ToAbsInput(roughFile);
    std::string metalAbs = ToAbsInput(metalFile);
    std::string glossAbs = ToAbsInput(glossyFile);

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
    rawMat.textures.push_back({ ormFileName, RawTextureType::ORM });
}

