#include "ResourcePch.h"
#include "StaticMesh.h"
#include "Utils/SubMeshBuilder.h"

namespace MGF3D
{
	StaticMesh::StaticMesh(StaticMeshDescriptor& desc) : Super(desc) {}
	StaticMesh::~StaticMesh() = default;

	/*==================================//
	//         Type System              //
	//==================================*/
	UniquePtr<MGFType> StaticMesh::s_type = nullptr;

	void StaticMesh::InitializeType()
	{
		s_type = MakeUnique<MGFType>();
		s_type->name = MGFName("StaticMesh");
		s_type->id = StringHash("StaticMesh");

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

	const MGFType* StaticMesh::GetType() const
	{
		return s_type.Get();
	}

	StaticMeshPtr StaticMesh::Create(StaticMeshDescriptor&& desc)
	{
		auto mesh = StaticMeshPtr(new StaticMesh(desc));
		if (!mesh->Init(std::move(desc))) return nullptr;
		return mesh;
	}

	bool StaticMesh::Init(StaticMeshDescriptor&& desc)
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

	bool StaticMesh::OnLoad()
	{
		if (m_meshData.Count() == 0)
		{
			MGF_LOG_ERROR("[StaticMesh] No geometry data available for Load.");
			return false;
		}

		ComputeTangents();
		return true;
	}

	bool StaticMesh::OnCommit()
	{
		if (m_meshData.Count() == 0) return false;

		// 1. 모든 기하 데이터를 순회하며 SubMesh 생성
		bool bSuccess = false;
		for (auto& geoData : m_meshData)
		{
			if (geoData.vertices.Count() == 0 || geoData.indices.Count() == 0) continue;

			// VRAM 버퍼 및 레이아웃 생성
			SubMesh subMesh = SubMeshBuilder::BuildSubMesh<StaticVertex>(geoData.vertices, geoData.indices);
			if (!subMesh.vertexBuffer || !subMesh.indexBuffer)
			{
				MGF_LOG_ERROR("[StaticMesh] Failed to create VRAM buffers for a submesh.");
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

	void StaticMesh::OnRelease()
	{
		m_meshData.Clear();
	}

	void StaticMesh::ComputeTangents()
	{
		// 1. 모든 서브 메쉬(GeometryData)를 순회
		for (auto& geoData : m_meshData)
		{
			auto& vertices = geoData.vertices;
			auto& indices = geoData.indices;
			if (vertices.Count() == 0 || indices.Count() == 0) continue;

			// 2. 임시 탄젠트 누적 버퍼 생성
			LVector<vec3> tangents;
			tangents.Resize(vertices.Count(), vec3(0.0f));

			// 3. 인덱스 버퍼를 참조하여 삼각형 단위로 탄젠트 계산
			for (usize i = 0; i < indices.Count(); i += 3)
			{
				uint32 i0 = indices[i];
				uint32 i1 = indices[i + 1];
				uint32 i2 = indices[i + 2];

				StaticVertex& v0 = vertices[i0];
				StaticVertex& v1 = vertices[i1];
				StaticVertex& v2 = vertices[i2];

				// 위치 차이 (Edge 벡터)
				vec3 edge1 = v1.position - v0.position;
				vec3 edge2 = v2.position - v0.position;

				// UV 차이 (Delta UV)
				vec2 deltaUV1 = v1.texCoord - v0.texCoord;
				vec2 deltaUV2 = v2.texCoord - v0.texCoord;

				mat2 uvMat(deltaUV1, deltaUV2);
				float det = Math::Determinant(uvMat);
			
				vec3 t;
				if (!Math::IsNearlyZero(det))
				{
					float invDet = 1.0f / det;
					t.x = invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
					t.y = invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
					t.z = invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				}
				else
				{
					// Determinant가 0인 경우 (UV가 겹치거나 잘못된 경우) 에지 방향으로 대체
					t = edge1;
				}

				// 각 정점에 계산된 탄젠트 누적
				tangents[i0] += t;
				tangents[i1] += t;
				tangents[i2] += t;
			}

			// 4. 누적된 탄젠트를 정규화하고 노멀과 직교화
			for (usize i = 0; i < vertices.Count(); ++i)
			{
				const glm::vec3& n = vertices[i].normal;
				const glm::vec3& t = tangents[i];

				if (Math::HasLength(t))
				{
					// 그람-슈미트 직교화: T' = normalize(T - N * dot(N, T))
					// 탄젠트를 노멀 방향 성분을 제거하여 완벽한 수직으로 만듭니다.
					vertices[i].tangent = Math::Normalize(t - n * Math::Dot(n, t));
				}
				else
				{
					// 유효한 탄젠트를 구할 수 없는 경우 기본축 설정
					vertices[i].tangent = vec3(1.0f, 0.0f, 0.0f);
				}
			}
		}
	}
}