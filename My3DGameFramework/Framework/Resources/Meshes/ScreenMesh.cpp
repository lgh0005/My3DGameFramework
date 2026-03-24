#include "ResourcePch.h"
#include "ScreenMesh.h"
#include "Utils/SubMeshBuilder.h"

namespace MGF3D
{
    ScreenMesh::ScreenMesh(ScreenMeshDescriptor& desc) : Super(desc) { }
    ScreenMesh::~ScreenMesh() = default;

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