#include "ResourcePch.h"
#include "SkinnedMesh.h"
#include "Utils/SubMeshBuilder.h"

namespace MGF3D
{
	SkinnedMesh::SkinnedMesh(SkinnedMeshDescriptor& desc) : Super(desc) {}
	SkinnedMesh::~SkinnedMesh() = default;

	/*==================================//
	//         Type System              //
	//==================================*/
	UniquePtr<MGFType> SkinnedMesh::s_type = nullptr;

	void SkinnedMesh::InitializeType()
	{
		s_type = MakeUnique<MGFType>();
		s_type->name = MGFName("SkinnedMesh");
		s_type->id = StringHash("SkinnedMesh");

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

	const MGFType* SkinnedMesh::GetType() const
	{
		return s_type.Get();
	}

	SkinnedMeshPtr SkinnedMesh::Create(SkinnedMeshDescriptor&& desc)
	{
		auto mesh = SkinnedMeshPtr(new SkinnedMesh(desc));
		if (!mesh->Init(std::move(desc))) return nullptr;
		return mesh;
	}

	bool SkinnedMesh::Init(SkinnedMeshDescriptor&& desc)
	{
		if (desc.geometryList.Count() == 0)
		{
			MGF_LOG_ERROR("[SkinnedMesh] Geometry list is empty in Descriptor.");
			return false;
		}
		m_meshData = std::move(desc.geometryList);
		return true;
	}

	bool SkinnedMesh::OnLoad()
	{
		if (m_meshData.Count() == 0) return false;

		// 1. 컨버터에서 탄젠트가 베이킹되었는지 확인
		bool bNeedCompute = false;
		for (const auto& geo : m_meshData)
		{
			if (geo.vertices.Count() > 0)
			{
				// 첫 번째 정점의 탄젠트가 유효하지 않으면 계산이 필요한 것으로 간주
				if (!Math::HasLength(geo.vertices[0].tangent))
				{
					bNeedCompute = true;
					break;
				}
			}
		}

		// 2. 필요 시 워커 스레드에서 탄젠트 연산 수행
		if (bNeedCompute)
		{
			MGF_LOG_INFO("[SkinnedMesh] Computing missing tangents in background...");
			ComputeTangents();
		}

		return true;
	}

	bool SkinnedMesh::OnCommit()
	{
		if (m_meshData.Count() == 0) return false;

		// 메인 스레드에서 GPU 버퍼 생성 및 데이터 업로드
		bool bSuccess = false;
		for (auto& geoData : m_meshData)
		{
			if (geoData.vertices.Count() == 0 || geoData.indices.Count() == 0) continue;

			// SubMeshBuilder에서 이전에 작성한 SkinnedVertex 전용 레이아웃 설정을 호출합니다.
			SubMesh subMesh = SubMeshBuilder::BuildSubMesh<SkinnedVertex>(geoData.vertices, geoData.indices);
			if (!subMesh.vertexBuffer || !subMesh.indexBuffer)
			{
				MGF_LOG_ERROR("[SkinnedMesh] Failed to create VRAM buffers for a submesh.");
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

	void SkinnedMesh::OnRelease()
	{
		m_meshData.Clear();
	}

	void SkinnedMesh::ComputeTangents()
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

				SkinnedVertex& v0 = vertices[i0];
				SkinnedVertex& v1 = vertices[i1];
				SkinnedVertex& v2 = vertices[i2];

				// 위치 차이 (Edge 벡터)
				vec3 edge1 = v1.position - v0.position;
				vec3 edge2 = v2.position - v0.position;

				// UV 차이 (Delta UV)
				vec2 deltaUV1 = v1.texCoord - v0.texCoord;
				vec2 deltaUV2 = v2.texCoord - v0.texCoord;

				// Math 유틸리티를 이용한 2x2 행렬식 계산
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
				const vec3& n = vertices[i].normal;
				const vec3& t = tangents[i];

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