#include "ResourcePch.h"
#include "UIMesh.h"
#include "Utils/SubMeshBuilder.h"

namespace MGF3D
{
	UIMesh::UIMesh(UIMeshDescriptor& desc) : Super(desc) {}
	UIMesh::~UIMesh() = default;

	/*==================================//
	//         Type System              //
	//==================================*/
	UniquePtr<MGFType> UIMesh::s_type = nullptr;

	void UIMesh::InitializeType()
	{
		s_type = MakeUnique<MGFType>();
		s_type->name = MGFName("UIMesh");
		s_type->id = StringHash("UIMesh");

		// 부모인 Mesh의 타입 상속
		const MGFType* parentType = Mesh::s_type.Get();
		if (parentType)
		{
			s_type->parent = parentType;
			s_type->depth = parentType->depth + 1;

			for (uint32 i = 0; i <= parentType->depth; ++i)
				s_type->chain[i] = parentType->chain[i];
		}
		s_type->chain[s_type->depth] = s_type->id;
	}

	const MGFType* UIMesh::GetType() const
	{
		return s_type.Get();
	}

	UIMeshPtr UIMesh::Create(UIMeshDescriptor&& desc)
	{
		auto mesh = UIMeshPtr(new UIMesh(desc));
		if (!mesh->Init(std::move(desc))) return nullptr;
		return mesh;
	}

	bool UIMesh::Init(UIMeshDescriptor&& desc)
	{
		// 데이터가 비어있는지 확인
		if (desc.geometryList.Count() == 0)
		{
			MGF_LOG_ERROR("[StaticMesh] Geometry list is empty in Descriptor.");
			return false;
		}

		// Descriptor로부터 데이터 소유권 이전
		m_meshData = std::move(desc.geometryList);
		return true;
	}

	bool UIMesh::OnLoad()
	{
		return true;
	}

	bool UIMesh::OnCommit()
	{
		if (m_meshData.Count() == 0) return false;

		bool bSuccess = false;
		for (auto& geoData : m_meshData)
		{
			if (geoData.vertices.Count() == 0 || geoData.indices.Count() == 0) continue;

			// SubMeshBuilder에서 UIVertex 전용 레이아웃 설정 호출
			SubMesh subMesh = SubMeshBuilder::BuildSubMesh<UIVertex>(geoData.vertices, geoData.indices);
			if (!subMesh.vertexBuffer || !subMesh.indexBuffer)
			{
				MGF_LOG_ERROR("[UIMesh] Failed to create VRAM buffers for a submesh.");
				return false;
			}

			// 머티리얼 정보 이식
			subMesh.materialIndex = geoData.materialIndex;

			// 부모 Mesh 컨테이너에 추가
			AddSubMesh(std::move(subMesh));
			bSuccess = true;
		}

		return bSuccess;
	}

	void UIMesh::OnRelease()
	{
		m_meshData.Clear();
	}
}