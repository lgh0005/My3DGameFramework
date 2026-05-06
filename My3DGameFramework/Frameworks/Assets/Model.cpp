#include "FrameworkPch.h"
#include "Model.h"
#include "Managers/TypeManager.h"
#include "Managers/AssetManager.h"
#include "Managers/EntityManager.h"
#include "CoreUtils/AssetUtils.h"
#include "Components/Transform.h"
#include "Components/MeshRenderers/MeshRenderer.h"
#include "Components/MeshRenderers/SkinnedMeshRenderer.h"
#include "Mechanics/Animation/Skeleton.h"
#include "Graphics/Meshes/StaticMesh.h"
#include "Graphics/Meshes/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Assets/Image.h"
#include "Mechanics/Animation/Pose.h"

namespace MGF3D
{
	Model::Model(const String& path) : Super(path) { }
	Model::~Model() = default;

	/*========================//
	//       Image Type       //
	//========================*/
	int16 Model::s_typeIndex = -1;
	const MGFType* Model::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Asset");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	ModelPtr Model::Create(const String& path)
	{
		return ModelPtr(new Model(path));
	}

	bool Model::Load()
	{
		InputFileStream inFile(m_path, std::ios::binary);
		if (!inFile)
		{
			MGF_LOG_ERROR("Model::Load - Failed to open file: {}", m_path);
			SetState(EAssetState::Failed);
			return false;
		}

		// 1. Header 읽기
		uint32 magic = AssetUtils::ReadData<uint32>(inFile);
		if (magic != 0x4D594D44) // "MYMD"
		{
			SetState(EAssetState::Failed);
			return false;
		}

		uint32 version = AssetUtils::ReadData<uint32>(inFile);
		uint32 matCount = AssetUtils::ReadData<uint32>(inFile);
		uint32 meshCount = AssetUtils::ReadData<uint32>(inFile);
		bool hasSkeleton = AssetUtils::ReadData<bool>(inFile);
		m_aabbMin = AssetUtils::ReadData<vec3>(inFile);
		m_aabbMax = AssetUtils::ReadData<vec3>(inFile);

		// 2. Hierarchy 읽기 (멤버 변수에 캐싱)
		m_nodes = AssetUtils::ReadRawNodes(inFile);

		// 3. Skeleton 파싱 및 생성
		if (hasSkeleton)
		{
			uint32 boneCount = AssetUtils::ReadData<uint32>(inFile);
			Vector<mat4> offsets(boneCount);
			Vector<int32> parentIndices(boneCount, -1);

			// Bone Offset Info 파싱
			for (uint32 i = 0; i < boneCount; ++i)
			{
				int32 id = AssetUtils::ReadData<int32>(inFile);
				mat4 offset = AssetUtils::ReadData<mat4>(inFile);
				if (id >= 0 && id < boneCount) offsets[id] = offset;
			}

			// Bone Name to ID Map 파싱 (문자열 읽자마자 StringHash로 변환)
			uint32 mapCount = AssetUtils::ReadData<uint32>(inFile);
			HashMap<StringHash, int32> boneMap;
			boneMap.reserve(mapCount);

			for (uint32 i = 0; i < mapCount; ++i)
			{
				String boneName = AssetUtils::ReadString(inFile);
				int32 id = AssetUtils::ReadData<int32>(inFile);
				boneMap[StringHash(boneName)] = id;
			}

			// Parent Indices 구축 (GPU 연산용 배열)
			for (const auto& node : m_nodes)
			{
				auto myIt = boneMap.find(StringHash(node.name));
				if (myIt != boneMap.end())
				{
					int32 myBoneID = myIt->second;
					int32 searchParentIdx = node.parentIndex;

					// 부모가 진짜 뼈일 때까지 계속 위로 거슬러 올라감
					for (const auto& node : m_nodes)
					{
						auto myIt = boneMap.find(StringHash(node.name));
						if (myIt != boneMap.end())
						{
							int32 myBoneID = myIt->second;
							if (node.parentIndex >= 0)
							{
								const auto& parentNode = m_nodes[node.parentIndex];
								auto parentIt = boneMap.find(StringHash(parentNode.name));
								if (parentIt != boneMap.end()) parentIndices[myBoneID] = parentIt->second;
							}
						}
					}
				}
			}

			// 팩토리로 생성
			m_skeleton = Skeleton::Create(boneCount, std::move(offsets), std::move(parentIndices), std::move(boneMap));
		}

		// 4. Materials 읽기
		m_materials.reserve(matCount);
		for (uint32 i = 0; i < matCount; ++i)
		{
			RawMaterial rawMat = AssetUtils::ReadRawMaterial(inFile);

			// 4-1. Material 생성
			MaterialPtr material = Material::Create(rawMat.name);

			// 4-2. 파라미터 셋업
			material->SetAlbedoFactor(rawMat.albedoFactor);
			material->SetEmissiveFactor(rawMat.emissiveFactor);
			material->SetEmissionStrength(rawMat.emissiveStrength);
			material->SetMetallicFactor(rawMat.metallicFactor);
			material->SetRoughnessFactor(rawMat.roughnessFactor);

			// 4-3. 텍스처 의존성 셋업
			for (const auto& texInfo : rawMat.textures)
			{
				// m_path 문자열을 filesystem::path 객체로 변환
				FileSystem::path modelFilePath(m_path);
				FileSystem::path parentDir = modelFilePath.parent_path();
				FileSystem::path fullTexPath = parentDir / texInfo.fileName;
				String texPath = fullTexPath.string();

				// Image 에셋 비동기 로딩 파이프라인 진입
				ImagePtr imageAsset = MGF_ASSET.LoadAssetAsync<Image>(texPath);

				// 슬롯 매핑
				switch (texInfo.type)
				{
				case RawTextureType::Albedo: material->SetTexture(ETextureSlot::Albedo, imageAsset); break;
				case RawTextureType::Normal: material->SetTexture(ETextureSlot::Normal, imageAsset); break;
				case RawTextureType::Emissive: material->SetTexture(ETextureSlot::Emission, imageAsset); break;
				case RawTextureType::Height: material->SetTexture(ETextureSlot::Height, imageAsset); break;
				case RawTextureType::ORM: material->SetTexture(ETextureSlot::ORM, imageAsset); break;
				case RawTextureType::Specular: material->SetTexture(ETextureSlot::Specular, imageAsset); break;
				default: break;
				}
			}

			m_materials.push_back(material);
			AddResource(material);
		}

		// 5. Meshes 읽기
		m_meshes.reserve(meshCount);
		for (uint32 i = 0; i < meshCount; ++i)
		{
			RawMesh rawMesh = AssetUtils::ReadRawMesh(inFile);
			MeshPtr mesh;
			
			// 5-1. 메쉬 타입에 따른 생성
			if (rawMesh.isSkinned)
			{
				Vector<SkinnedVertex> engineVertices;
				engineVertices.resize(rawMesh.skinnedVertices.size());
				std::memcpy // TODO : 래핑해두면 좋을 수 있음
				(
					engineVertices.data(), rawMesh.skinnedVertices.data(),
					rawMesh.skinnedVertices.size() * sizeof(SkinnedVertex)
				);

				mesh = SkinnedMesh::Create(std::move(engineVertices), std::move(rawMesh.indices));
			}
			else
			{
				Vector<StaticVertex> engineVertices;
				engineVertices.resize(rawMesh.staticVertices.size());
				std::memcpy
				(
					engineVertices.data(), rawMesh.staticVertices.data(),
					rawMesh.staticVertices.size() * sizeof(StaticVertex)
				);

				mesh = StaticMesh::Create(std::move(engineVertices), std::move(rawMesh.indices));
			}

			if (mesh)
			{
				// 5-2. 바운딩 박스 세팅
				RenderBounds bounds;
				bounds.SetFromMinMax(rawMesh.aabbMin, rawMesh.aabbMax);
				mesh->SetLocalBounds(bounds);

				// 5-3. (선택적) 머티리얼 인덱스 저장. 
				mesh->SetMaterialIndex(rawMesh.materialIndex);

				m_meshes.push_back(mesh);
				AddResource(mesh);
			}
		}

		inFile.close();
		SetState(EAssetState::Loaded); // 파일 읽기(CPU 로딩) 성공
		return true;
	}

	ObjectIDHash Model::Instantiate(const String& name)
	{
		if (GetState() != EAssetState::Ready && GetState() != EAssetState::Loaded)
			return ObjectIDHash(0);

		// 1. 생성된 GameObject들의 ID를 추적하기 위한 컨테이너
		Vector<ObjectIDHash> spawnedIDs(m_nodes.size());

		// 2. 노드 계층 구조 순회 및 생성
		for (uint32 i = 0; i < (uint32)m_nodes.size(); ++i)
		{
			const auto& node = m_nodes[i];
			String targetName = node.name;

			// EntityManager를 통해 GameObject 생성 및 ID 획득
			ObjectIDHash currentID = MGF_ENTITY.CreateGameObject(targetName);
			spawnedIDs[i] = currentID;
			MGF_ENTITY.AddComponent<Transform>(currentID);

			// 4. Transform 설정
			Transform* transform = MGF_ENTITY.GetComponent<Transform>(currentID);
			if (transform)
			{
				// 2. 부모-자식 Transform 계층 연결
				if (i > 0 && node.parentIndex != -1)
				{
					ObjectIDHash parentID = spawnedIDs[node.parentIndex];
					Transform* parentTransform = MGF_ENTITY.GetComponent<Transform>(parentID);
					if (parentTransform) transform->SetParent(parentTransform);
				}

				// 3. Local Matrix를 개별 TRS 성분으로 분해하여 주입
				Pose localPose = Pose::FromMat4(node.localTransform);
				transform->SetLocalPosition(localPose.position);
				transform->SetLocalRotation(localPose.rotation);
				transform->SetLocalScale(localPose.scale);
			}

			// 5. 메쉬 및 렌더러 컴포넌트 조립
			for (uint32 m = 0; m < (uint32)node.meshIndices.size(); ++m)
			{
				uint32 meshIdx = node.meshIndices[m];
				if (meshIdx >= m_meshes.size()) continue;

				auto mesh = m_meshes[meshIdx];
				uint32 matIdx = mesh->GetMaterialIndex();

				MaterialPtr targetMaterial = nullptr;
				if (matIdx < m_materials.size()) targetMaterial = m_materials[matIdx];

				String meshObjName = targetName + "_SubMesh_" + std::to_string(m);
				ObjectIDHash meshObjID = MGF_ENTITY.CreateGameObject(meshObjName);

				MGF_ENTITY.AddComponent<Transform>(meshObjID);
				Transform* meshTransform = MGF_ENTITY.GetComponent<Transform>(meshObjID);
				if (meshTransform)
				{
					Transform* rootTransform = MGF_ENTITY.GetComponent<Transform>(spawnedIDs[0]);
					if (rootTransform) meshTransform->SetParent(rootTransform);
				}

				auto skinnedMesh = MGFTypeCaster::Cast<SkinnedMesh>(mesh);
				if (skinnedMesh != nullptr)
				{
					auto* renderer = MGF_ENTITY.AddComponent<SkinnedMeshRenderer>(meshObjID, skinnedMesh, targetMaterial);
					if (renderer) renderer->SetRootEntityID(spawnedIDs[0]);
				}
				else
				{
					MGF_ENTITY.AddComponent<MeshRenderer>(meshObjID, mesh, targetMaterial);
				}
			}
		}

		return spawnedIDs[0];
	}
}