#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Meshes/Mesh.h"

// TODO : 지금은 StaticMesh에 대해서만 인스턴싱을 하고 있는데
// 이후에는 SkinnedMesh에 대해서도 인스턴싱을 할 수 있어야 한다.
#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
#pragma endregion

CLASS_PTR(InstancedMesh)
class InstancedMesh : public Mesh
{
    DEFINE_RESOURCE_TYPE(ResourceType::InstancedMesh, InstancedMeshDesc)

public:
    // 인스턴스 속성에 따른 람다 함수
    using SetupFunc =
        std::function<void(VertexLayout* layout,
            uint32 instanceAttribStartIndex)>;

    virtual ~InstancedMesh();
    static InstancedMeshUPtr Create
    (
        StaticMeshPtr baseMesh,           
        BufferPtr     instanceBuffer,  
        int32         instanceCount,
        SetupFunc setupFunc
    );
	virtual void Draw() const override;
    int32 GetInstanceCount() const { return m_instanceCount; }

private:
    InstancedMesh();
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