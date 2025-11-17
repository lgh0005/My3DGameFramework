#pragma once
#include "Graphics/Mesh.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
#pragma endregion

CLASS_PTR(InstancedMesh)
class InstancedMesh : public Mesh
{
public:
    // 인스턴스 속성에 따른 람다 함수
    using SetupFunc =
        std::function<void(VertexLayout* layout,
            uint32 instanceAttribStartIndex)>;

    static InstancedMeshUPtr Create
    (
        StaticMeshPtr baseMesh,           
        BufferPtr     instanceBuffer,  
        int32         instanceCount,
        SetupFunc setupFunc
    );
	virtual ~InstancedMesh() = default;
    virtual MeshType GetMeshType() const override { return MeshType::Instanced; }
	virtual void Draw(const Program* program) const override;

private:
    InstancedMesh() = default;
    bool Init
    (
        StaticMeshPtr baseMesh,
        BufferPtr instancePositionBuffer,
        int32 instanceCount,
        SetupFunc setupFunc
    );

    BufferPtr m_instanceBuffer;
    int32     m_instanceCount    { 0 };

/*=========================//
//  instance method utils  //
//=========================*/
public:
    static SetupFunc Vec3Layout();
    static SetupFunc Mat4Layout();
};