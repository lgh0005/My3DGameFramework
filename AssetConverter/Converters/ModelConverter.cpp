#include "ConverterPch.h"
#include "ModelConverter.h"
#include "KTXTextureConverter.h"
#include "ORMTexturePacker.h"
#include "Managers/ThreadManager.h"

namespace MGF3D
{
    ModelConverter::ModelConverter() = default;
    ModelConverter::~ModelConverter() = default;

    /*==========================================//
    //  [Public Entry] 변환 시작 및 상태 초기화   //
    //==========================================*/
    bool ModelConverter::Convert
    (
        const String& inputPath,
        const String& outputPath,
        bool extractORM, bool flipY
    )
    {
        // 상태 초기화 (State Clear)
        m_rawModel = RawModel();
        m_boneNameToIdMap.clear();
        m_boneCounter = 0;
        m_extractORM = extractORM;
        m_inputPath.clear();
        m_outputPath.clear();
        m_modelDirectory.clear();

        // 로그 출력 및 입출력 경로 캐싱
        MGF_LOG_INFO(" [ModelConverter] Start Conversion");
        MGF_LOG_INFO(" - Input:  {}", inputPath);
        MGF_LOG_INFO(" - Output: {}", outputPath);
        m_extractORM = extractORM;
        m_flipY = flipY;
        m_inputPath = inputPath;
        m_outputPath = outputPath;

        // 실제 로직 실행
        bool result = RunConversion();

        if (result) MGF_LOG_INFO(" [Success] Conversion Completed.");
        else MGF_LOG_ERROR(" [Failed] Conversion Aborted.");

        return result;
    }

    /*============================//
    //   main coversion methods   //
    //============================*/
    bool ModelConverter::RunConversion()
    {
        // 0. 초기화
        m_rawModel = RawModel(); // 모델 데이터 초기화
        m_boneNameToIdMap.clear();
        m_boneOffsets.clear();
        m_boneCounter = 0;

        // 0-1. 모델 이름과 상위 경로를 추출
        const FileSystem::path outPath(m_outputPath);
        m_modelDirectory = outPath.parent_path().string();
        m_modelName = outPath.stem().string();
        if (m_modelName.empty()) m_modelName = "unnamed_model";

        // 1. Assimp 임포터 실행
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile
        (
            m_inputPath,
            aiProcess_Triangulate |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_LimitBoneWeights |
            aiProcess_FlipUVs
        );
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            MGF_LOG_ERROR("Assimp ReadFile failed: {}", importer.GetErrorString());
            return false;
        }

        // 2. 머티리얼 처리
        MGF_LOG_INFO("Processing materials... Total: {}", scene->mNumMaterials);
        m_rawModel.materials.reserve(scene->mNumMaterials);
        for (uint32 i = 0; i < scene->mNumMaterials; ++i)
            m_rawModel.materials.push_back(ProcessMaterial(scene->mMaterials[i], i));

        // 3. 계층 구조 완성 (Hierarchy Extraction)
        MGF_LOG_INFO("Processing node hierarchy...");
        ParseNodeHierarchy(scene);
        MGF_LOG_INFO(" - Total Nodes extracted: {}", m_rawModel.nodes.size());

        // 3-1. 모든 메쉬를 미리 스캔하여 뼈 이름과 오프셋만 수집
        CollectBoneOffsets(scene);

        // 3-2. 노드 순서대로 뼈 ID 최종 확정
        FinalizeBoneIDs();

        // 4. 이제 확정된 ID를 바탕으로 메쉬 처리
        MGF_LOG_INFO("Processing meshes... Total: {}", scene->mNumMeshes);
        m_rawModel.meshes.reserve(scene->mNumMeshes);
        for (uint32 i = 0; i < scene->mNumMeshes; ++i)
            ProcessMesh(scene->mMeshes[i], scene);

        // 5. 스켈레톤 존재 여부 최종 확정
        if (m_rawModel.hasSkeleton) MGF_LOG_INFO(" - Skeleton Detected (Total Bones: {})", m_boneCounter);
        else MGF_LOG_INFO(" - Static Mesh Detected (No Skeleton)");

        // [추가] 모든 비동기 텍스처 작업 대기
        MGF_LOG_INFO("Waiting for texture conversion tasks...");
        WaitAllTasks();

        // 6. 파일 저장 (직렬화)
        MGF_LOG_INFO("Writing Binary File...");
        if (!WriteCustomModelFile())
        {
            MGF_LOG_ERROR("Failed to write output file: {}", m_outputPath);
            return false;
        }

        return true;
    }

    bool ModelConverter::WriteCustomModelFile()
    {
        MGF_LOG_INFO("Attempting to write file to: {}", m_outputPath);
        OutputFileStream outFile(m_outputPath, std::ios::binary);
        if (!outFile) return false;

        // [DEBUG] 시작 위치
        long pos = (long)outFile.tellp();
        MGF_LOG_WARN(">>> [WRITER] Start Offset: {}", pos);

        // 1. Header
        uint32 magic = 0x4D594D44; uint32 version = 3;
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
        MGF_LOG_WARN(">>> [WRITER] Header: {}", (long)outFile.tellp());

        // 2. Hierarchy
        MGF_LOG_WARN(">>> [WRITER] Node Hierarchy: {}", (long)outFile.tellp());
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
        MGF_LOG_WARN(">>> [WRITER] Before Materials: {}", (long)outFile.tellp());
        for (const auto& mat : m_rawModel.materials) AssetUtils::WriteRawMaterial(outFile, mat);

        // 4. Meshes
        MGF_LOG_WARN(">>> [WRITER] Before Meshes: {}", (long)outFile.tellp());
        for (const auto& mesh : m_rawModel.meshes) AssetUtils::WriteRawMesh(outFile, mesh);

        MGF_LOG_WARN(">>> [WRITER] Final Size: {}", (long)outFile.tellp());
        outFile.close();
        return true;
    }

    void ModelConverter::CreateORMTextureFromAssimp(aiMaterial* material, RawMaterial& rawMat, int32 index)
    {
        // 0. 옵션이 꺼져있으면 즉시 리턴
        if (!m_extractORM) return;

        // 1. 입력 텍스처 소스 경로 확보
        String aoFile = GetTexturePath(material, aiTextureType_AMBIENT_OCCLUSION);
        String roughFile = GetTexturePath(material, aiTextureType_DIFFUSE_ROUGHNESS);
        String metalFile = GetTexturePath(material, aiTextureType_METALNESS);
        String glossyFile = GetTexturePath(material, aiTextureType_SHININESS);
        if (aoFile.empty() && roughFile.empty() &&
            metalFile.empty() && glossyFile.empty()) return;

        String aoAbs = ResolveTexturePath(aoFile);
        String roughAbs = ResolveTexturePath(roughFile);
        String metalAbs = ResolveTexturePath(metalFile);
        String glossAbs = ResolveTexturePath(glossyFile);

        // 2. Roughness vs Glossiness 결정 및 인버트 여부
        String finalRoughPath = roughAbs;
        bool invertRoughness = false;
        if (finalRoughPath.empty() && !glossAbs.empty())
        {
            finalRoughPath = glossAbs;
            invertRoughness = true; // ORM Packer에게 "이거 뒤집어서 써라"라고 지시
            MGF_LOG_INFO("  - Roughness map missing. Using Glossiness map instead (Inverted).");
        }

        // 3. 최종 KTX 파일명 및 경로 정의
        String ormKtxName = fmt::format("{}_{}_ORM.ktx", m_modelName, index);
        FileSystem::path outputDir = FileSystem::path(m_outputPath).parent_path();
        FileSystem::path finalKtxPath = outputDir / ormKtxName;
        {
            LockScope lock(m_textureSetMutex);
            m_convertedTextures.insert(ormKtxName);
        }

        // 4. 비동기 ORM 처리
        AddTaskCount();
        MGF_THREAD.PushTask
        (
            [this, aoAbs, finalRoughPath, metalAbs, finalKtxPath, invertRoughness, ormKtxName]()
            {
                RawImage ormRaw;
                MGF_LOG_INFO("[ORM Packer Start] Packing textures... (FlipY: {})", m_flipY);

                if (!MGF_CONV_ORM.Pack(aoAbs, finalRoughPath, metalAbs, ormRaw, invertRoughness, m_flipY))
                {
                    MGF_LOG_ERROR("[ORM Packer] Failed to pack ORM pixels in memory.");
                    CompleteTaskCount();
                    return;
                }

                if (ormRaw.width <= 0 || ormRaw.height <= 0 || ormRaw.pixels.empty())
                {
                    MGF_LOG_ERROR("[ORM Packer] Invalid baked image size: {}x{}", ormRaw.width, ormRaw.height);
                    CompleteTaskCount();
                    return;
                }

                MGF_LOG_INFO("[ORM Packer] Converting memory buffer to KTX: {}", ormKtxName);
                bool ktxResult = MGF_CONV_KTX.ConvertFromMemory
                (
                    ormRaw.pixels.data(), ormRaw.width, ormRaw.height,
                    finalKtxPath.string(), "BC7", "Linear", false
                );

                if (!ktxResult) MGF_LOG_ERROR("[ORM Packer] KTX conversion failed for: {}", ormKtxName);
                else MGF_LOG_INFO("  [Success] ORM Texture baked and converted to KTX successfully.");

                CompleteTaskCount();
            }
        );

        // 6. 결과 기록 및 중복 변환 목록에 추가
        rawMat.textures.push_back({ ormKtxName, RawTextureType::ORM });

        MGF_LOG_INFO("  [Success] ORM Texture baked and converted to KTX successfully.");
    }

    /*====================================//
    //   default assimp process methods   //
    //====================================*/
    void ModelConverter::ParseNodeHierarchy(const aiScene* scene)
    {
        if (!scene->mRootNode) return;

        // 1. 작업을 관리할 구조체 정의
        struct NodeJob
        {
            aiNode* node;
            int32 parentIndex;
        };

        // 2. 작업 스택 생성 및 예약
        std::vector<NodeJob> workStack;
        workStack.reserve(128);

        // 3. 루트 노드 삽입 (부모 인덱스는 -1)
        workStack.push_back({ scene->mRootNode, -1 });

        // 4. 스택이 빌 때까지 반복 (DFS 순회)
        while (!workStack.empty())
        {
            // 스택에서 작업 하나 꺼내기 (LIFO)
            NodeJob job = workStack.back();
            workStack.pop_back();

            aiNode* currNode = job.node;
            int32 parentIndex = job.parentIndex;

            // RawNode 데이터 기록 시작
            RawNode rawNode;

            // 4-1. 이름 복사
            rawNode.name = currNode->mName.C_Str();
            rawNode.parentIndex = parentIndex;

            // 4-2. 행렬 변환
            rawNode.localTransform = Math::ConvertToGLMMat4(currNode->mTransformation);

            // 4-3. 메쉬 인덱스 복사
            if (currNode->mNumMeshes > 0)
            {
                rawNode.meshIndices.resize(currNode->mNumMeshes);
                for (uint32 i = 0; i < currNode->mNumMeshes; ++i)
                    rawNode.meshIndices[i] = currNode->mMeshes[i];
            }

            // 4-4. 현재 노드가 저장될 인덱스 (= 현재까지 저장된 노드 개수)
            int32 myIndex = (int32)m_rawModel.nodes.size();

            // 데이터 저장
            m_rawModel.nodes.push_back(rawNode);

            // 4-5. 부모 노드의 children 리스트에 '나(myIndex)'를 등록
            if (parentIndex >= 0)
                m_rawModel.nodes[parentIndex].children.push_back(myIndex);

            // 5. 자식 노드들을 스택에 추가
            for (int32 i = (int32)currNode->mNumChildren - 1; i >= 0; --i)
                workStack.push_back({ currNode->mChildren[i], myIndex });
        }
    }

    void ModelConverter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        RawMesh rawMesh;
        rawMesh.name = mesh->mName.C_Str();
        rawMesh.materialIndex = mesh->mMaterialIndex;

        // [LOG] 메쉬 기본 정보 출력
        MGF_LOG_INFO("Processing Mesh: '{}'", rawMesh.name);
        MGF_LOG_INFO("  - Material Index: {}", rawMesh.materialIndex);
        MGF_LOG_INFO("  - Vertex Count: {}", mesh->mNumVertices);
        MGF_LOG_INFO("  - Face Count: {}", mesh->mNumFaces);
        MGF_LOG_INFO("  - Bone Count: {}", mesh->mNumBones);
        MGF_LOG_INFO("  - Has TextureCoords: {}", mesh->HasTextureCoords(0) ? "Yes" : "No");
        MGF_LOG_INFO("  - Has Tangents: {}", mesh->HasTangentsAndBitangents() ? "Yes" : "No");

        // 1. 스킨드(애니메이션) 메쉬인지 판단
        rawMesh.isSkinned = (mesh->mNumBones > 0);
        MGF_LOG_INFO("  - Mesh Type: {}", rawMesh.isSkinned ? "Skinned Mesh" : "Static Mesh");

        // 2. 바운딩 박스 초기화 (최대값으로 뒤집어서 시작)
        rawMesh.aabbMin = glm::vec3(FLT_MAX);
        rawMesh.aabbMax = glm::vec3(-FLT_MAX);

        // 2. 타입별 정점 처리 (함수 분리)
        if (rawMesh.isSkinned) ProcessSkinnedMesh(mesh, rawMesh);
        else ProcessStaticMesh(mesh, rawMesh);

        // [LOG] AABB 결과 확인 (너무 크거나 작으면 스케일/단위 문제 의심)
        MGF_LOG_INFO("  - AABB Min: ({:.4f}, {:.4f}, {:.4f})", rawMesh.aabbMin.x, rawMesh.aabbMin.y, rawMesh.aabbMin.z);
        MGF_LOG_INFO("  - AABB Max: ({:.4f}, {:.4f}, {:.4f})", rawMesh.aabbMax.x, rawMesh.aabbMax.y, rawMesh.aabbMax.z);

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

    void ModelConverter::ProcessSkinnedMesh(aiMesh* mesh, RawMesh& rawMesh)
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
            rawMesh.aabbMin = Math::Min(rawMesh.aabbMin, v.position);
            rawMesh.aabbMax = Math::Max(rawMesh.aabbMax, v.position);
            m_rawModel.globalAABBMin = Math::Min(m_rawModel.globalAABBMin, v.position);
            m_rawModel.globalAABBMax = Math::Max(m_rawModel.globalAABBMax, v.position);
        }

        // 2. 뼈 가중치 추출 (이미 별도 함수로 분리되어 있음)
        ExtractBoneWeights(rawMesh.skinnedVertices, mesh);
    }

    void ModelConverter::ProcessStaticMesh(aiMesh* mesh, RawMesh& rawMesh)
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
            rawMesh.aabbMin = Math::Min(rawMesh.aabbMin, v.position);
            rawMesh.aabbMax = Math::Max(rawMesh.aabbMax, v.position);
            m_rawModel.globalAABBMin = Math::Min(m_rawModel.globalAABBMin, v.position);
            m_rawModel.globalAABBMax = Math::Max(m_rawModel.globalAABBMax, v.position);
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
        MGF_LOG_INFO("  [Factors]");
        MGF_LOG_INFO("   - Albedo Color: ({:.2f}, {:.2f}, {:.2f}, {:.2f})", rawMat.albedoFactor.r, rawMat.albedoFactor.g, rawMat.albedoFactor.b, rawMat.albedoFactor.a);
        MGF_LOG_INFO("   - Metallic: {:.2f}, Roughness: {:.2f}", rawMat.metallicFactor, rawMat.roughnessFactor);

        // 2. 텍스처 추출 (Helper Lambda 활용)

        // Base Color (우선순위 처리)
        String baseColorPath = GetTexturePath(material, aiTextureType_BASE_COLOR);
        if (!baseColorPath.empty()) rawMat.textures.push_back({ baseColorPath, RawTextureType::Albedo });
        else AddTextureToMaterial(rawMat, material, aiTextureType_DIFFUSE, RawTextureType::Albedo);
        AddTextureToMaterial(rawMat, material, aiTextureType_NORMALS, RawTextureType::Normal);
        AddTextureToMaterial(rawMat, material, aiTextureType_HEIGHT, RawTextureType::Height);
        AddTextureToMaterial(rawMat, material, aiTextureType_SPECULAR, RawTextureType::Specular);
        AddTextureToMaterial(rawMat, material, aiTextureType_EMISSIVE, RawTextureType::Emissive);

        if (!m_extractORM)
        {
            AddTextureToMaterial(rawMat, material, aiTextureType_AMBIENT_OCCLUSION, RawTextureType::AmbientOcclusion);
            AddTextureToMaterial(rawMat, material, aiTextureType_DIFFUSE_ROUGHNESS, RawTextureType::Roughness);
            AddTextureToMaterial(rawMat, material, aiTextureType_METALNESS, RawTextureType::Metallic);
            AddTextureToMaterial(rawMat, material, aiTextureType_SHININESS, RawTextureType::Glossiness);
        }
        else
        {
            CreateORMTextureFromAssimp(material, rawMat, index);
        }

        // 4. 텍스쳐 매핑 로깅
        LogFinalMappedTextures(material, rawMat);

        return rawMat;
    }

    /*=========================================//
    //   texture helper methods for material   //
    //=========================================*/
    String ModelConverter::ResolveTexturePath(const String& relativePath)
    {
        if (relativePath.empty()) return "";

        FileSystem::path p(relativePath);
        FileSystem::path inputDir = FileSystem::path(m_inputPath).parent_path();

        // 1. 이미 절대 경로이고 존재하는 경우
        if (p.is_absolute() && FileSystem::exists(p)) return p.string();

        // 2. 입력 파일과 같은 폴더에 있는 경우
        if (FileSystem::exists(inputDir / p)) return (inputDir / p).string();

        // 3. 파일명만 떼서 같은 폴더에 있는지 확인 (경로가 꼬였을 때 대비)
        if (FileSystem::exists(inputDir / p.filename())) return (inputDir / p.filename()).string();

        return ""; // 못 찾음
    }

    void ModelConverter::AddTextureToMaterial(RawMaterial& rawMat, aiMaterial* aiMat, aiTextureType aiType, RawTextureType rawType)
    {
        // 1. Assimp 머티리얼에서 텍스처 파일명(경로) 가져오기
        String path = GetTexturePath(aiMat, aiType);

        // 2. 파일명이 비어있지 않다면 KTX 변환 및 등록 절차 진행
        if (!path.empty())
            ProcessTextureToKTX(rawMat, path, rawType);
    }

    String ModelConverter::GetTexturePath(aiMaterial* material, aiTextureType type)
    {
        if (material->GetTextureCount(type) <= 0) return "";

        aiString filepath;
        if (material->GetTexture(type, 0, &filepath) != AI_SUCCESS) return "";

        // 파일 이름만 추출 (경로 정규화)
        std::filesystem::path fullPath(filepath.C_Str());
        String filename = fullPath.filename().string();
        if (filename.empty()) return "";

        // [설계 결정] 리턴값은 "파일 이름"만.
        // INFO : 반드시 .mymodel과 이를 이루는 모든 텍스쳐는 같은 경로에 있게 된다.
        return filename;
    }

    void ModelConverter::LogFinalMappedTextures(aiMaterial* material, const RawMaterial& rawMat)
    {
        MGF_LOG_INFO("  [Textures] Count: {}", rawMat.textures.size());
        for (const auto& tex : rawMat.textures)
        {
            String typeStr;
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
            MGF_LOG_INFO("   - {:<12} : {}", typeStr, tex.fileName);
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
                MGF_LOG_TRACE("   [Assimp Debug] Found {:<15} : {}", check.name, debugPath.C_Str());
        }
    }

    void ModelConverter::ProcessTextureToKTX
    (
        RawMaterial& rawMat,
        const String& srcFileName,
        RawTextureType type
    )
    {
        if (srcFileName.empty()) return;

        // 1. 결과 파일명 결정 (brick.png -> brick.ktx)
        FileSystem::path srcPath(srcFileName);
        String ktxFileName = srcPath.stem().string() + ".ktx";
        FileSystem::path ktxAbsPath = FileSystem::path(m_outputPath).parent_path() / ktxFileName;

        // 2. 중복 변환 체크
        bool isAlreadyConverted = false;
        {
            LockScope lock(m_textureSetMutex);
            if (m_convertedTextures.find(ktxFileName) != m_convertedTextures.end()) isAlreadyConverted = true;
            else m_convertedTextures.insert(ktxFileName);
        }

        if (!isAlreadyConverted)
        {
            String srcAbsPath = ResolveTexturePath(srcFileName);
            if (!srcAbsPath.empty())
            {
                String colorSpace = "Linear";
                if (type == RawTextureType::Albedo || type == RawTextureType::Emissive)
                    colorSpace = "sRGB";

                AddTaskCount();
                MGF_THREAD.PushTask
                (
                    [this, srcAbsPath, ktxAbsPath, colorSpace, srcFileName, ktxFileName]()
                    {
                        MGF_LOG_INFO("  [KTX Export Start] {} -> {} ({})", srcFileName, ktxFileName, colorSpace);
                        if (!MGF_CONV_KTX.Convert(srcAbsPath, ktxAbsPath.string(), "BC7", colorSpace, m_flipY))
                            MGF_LOG_ERROR("  [KTX Export Failed] Failed to convert: {}", srcFileName);
                        
                        CompleteTaskCount();
                    }
                );
            }
        }

        rawMat.textures.push_back({ ktxFileName, type });
    }

    /*==============================//
    //   skeleton process methods   //
    //==============================*/
    void ModelConverter::ExtractBoneWeights(Vector<RawSkinnedVertex>& vertices, aiMesh* mesh)
    {
        // 메쉬가 가진 모든 뼈를 순회
        for (uint32 i = 0; i < mesh->mNumBones; ++i)
        {
            aiBone* bone = mesh->mBones[i];
            String boneName = bone->mName.C_Str();

            // 1. 이미 FinalizeBoneIDs에서 생성된 ID를 찾아서 사용
            auto it = m_boneNameToIdMap.find(boneName);

            // 계층 구조에는 없는데 메쉬에는 있는 뼈 (매우 드문 케이스, 더미 본 등)
            if (it == m_boneNameToIdMap.end())
            {
                MGF_LOG_WARN("Bone '{}' found in mesh but not in hierarchy! Skipping.", boneName);
                continue;
            }

            int32 boneID = it->second;

            // 2. 가중치 주입 (기존 로직 동일)
            for (uint32 j = 0; j < bone->mNumWeights; ++j)
            {
                const auto& weightData = bone->mWeights[j];
                uint32 vertexId = weightData.mVertexId;
                float weight = weightData.mWeight;

                // 가중치가 너무 작거나 인덱스가 벗어나면 무시
                if (weight <= 0.0f) continue;
                if (vertexId >= vertices.size()) continue;

                auto& v = vertices[vertexId];

                // 빈 슬롯(-1)을 찾아 채움
                for (int k = 0; k < MAX_BONE_INFLUENCE; ++k)
                {
                    if (v.boneIDs[k] < 0) // -1이면 비어있다는 뜻
                    {
                        v.boneIDs[k] = boneID;
                        v.weights[k] = weight;
                        break; // 채웠으면 다음 슬롯 검사 중단
                    }
                }
            }
        }
    }

    void ModelConverter::CollectBoneOffsets(const aiScene* scene)
    {
        for (uint32 i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[i];
            if (mesh->mNumBones > 0)
            {
                for (uint32 b = 0; b < mesh->mNumBones; ++b)
                {
                    aiBone* bone = mesh->mBones[b];
                    m_boneOffsets[bone->mName.C_Str()] = Math::ConvertToGLMMat4(bone->mOffsetMatrix);
                }
            }
        }
    }

    void ModelConverter::FinalizeBoneIDs()
    {
        // m_rawModel.nodes는 이미 ParseNodeHierarchy에 의해 DFS 순서(부모->자식)로 정렬됨
        for (const auto& node : m_rawModel.nodes)
        {
            // 수집된 뼈 오프셋 맵에 이 노드 이름이 존재하는가?
            auto it = m_boneOffsets.find(node.name);
            if (it != m_boneOffsets.end())
            {
                // 존재한다면, 이 노드는 '뼈'임. ID 부여.
                int32 newID = m_boneCounter++;

                RawBoneInfo info;
                info.id = newID;
                info.offset = it->second;

                m_rawModel.boneOffsetInfos.push_back(info);
                m_boneNameToIdMap[node.name] = newID;
            }
        }

        m_rawModel.hasSkeleton = (m_boneCounter > 0);
        MGF_LOG_INFO("Skeleton ID Finalized: Total {} bones mapped in hierarchy order.", m_boneCounter);
    }

    /*=============================//
    //   multi-threading members   //
    //=============================*/
    void ModelConverter::AddTaskCount()
    {
        ++m_activeTasks;
    }

    void ModelConverter::CompleteTaskCount()
    {
        --m_activeTasks;
        if (m_activeTasks == 0)  m_taskCv.notify_all();
    }

    void ModelConverter::WaitAllTasks()
    {
        UniqueLock lock(m_taskMutex);
        m_taskCv.wait(lock, [this]() { return m_activeTasks.load() == 0; });
    }
}