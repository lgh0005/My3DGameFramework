#include "ResourcePch.h"
#include "ScreenMesh.h"
#include "Utils/SubMeshBuilder.h"

namespace MGF3D
{
    ScreenMesh::ScreenMesh(ScreenMeshDescriptor& desc) : Super(desc) { }
    ScreenMesh::~ScreenMesh() = default;

    /*==================================//
    //         Type System              //
    //==================================*/
    UniquePtr<MGFType> ScreenMesh::s_type = nullptr;

    void ScreenMesh::InitializeType()
    {
        s_type = MakeUnique<MGFType>();
        s_type->name = MGFName("ScreenMesh");
        s_type->id = StringHash("ScreenMesh");

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

    const MGFType* ScreenMesh::GetType() const
    {
        return s_type.Get();
    }

    ScreenMeshPtr ScreenMesh::Create(ScreenMeshDescriptor&& desc)
    {
        auto mesh = ScreenMeshPtr(new ScreenMesh(desc));
        if (!mesh->Init(std::move(desc))) return nullptr;
        return mesh;
    }

    bool ScreenMesh::Init(ScreenMeshDescriptor&& desc)
    {
        if (desc.geometryList.Count() == 0) return false;
        m_meshData = std::move(desc.geometryList);
        return true;
    }

    bool ScreenMesh::OnLoad()
    {
        // nothing to do
        return true;
    }

    bool ScreenMesh::OnCommit()
    {
        if (m_meshData.Count() == 0) return false;

        // 메인 스레드에서 GPU 버퍼 생성 및 데이터 업로드
        bool bSuccess = false;
        for (auto& geoData : m_meshData)
        {
            if (geoData.vertices.Count() == 0 || geoData.indices.Count() == 0) continue;

            SubMesh subMesh = SubMeshBuilder::BuildSubMesh<ScreenVertex>(geoData.vertices, geoData.indices);
            if (!subMesh.vertexBuffer || !subMesh.indexBuffer)
            {
                MGF_LOG_ERROR("[ScreenMesh] Failed to create VRAM buffers for a submesh.");
                return false;
            }

            // 부모 Mesh 컨테이너에 추가
            AddSubMesh(std::move(subMesh));
            bSuccess = true;
        }

        return bSuccess;
    }

    void ScreenMesh::OnRelease()
    {
        m_meshData.Clear();
    }
}