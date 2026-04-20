#pragma once
#include "Meshes/Mesh.h"
#include "Vertices/SkinnedVertex.h"

namespace MGF3D
{
    MGF_CLASS_PTR(SkinnedMesh)
    class SkinnedMesh : public Mesh
    {
        using Super = Mesh;

    public:
        virtual ~SkinnedMesh() override;
        static SkinnedMeshPtr Create
        (
            Vector<SkinnedVertex>&& vertices,
            Vector<uint32>&& indices,
            uint32 primitiveType = GL_TRIANGLES
        );
        virtual bool OnSyncCreate() override;
        virtual void Draw(uint32 count) const override;

    /*======================//
    //   SkinnedMesh Type   //
    //======================*/
    public:
        static int16 s_typeIndex;
        virtual const MGFType* GetType() const override;

    public:
        void ComputeTangents
        (
            Vector<SkinnedVertex>& vertices,
            const Vector<uint32>& indices
        );

    private:
        SkinnedMesh();

        Vector<SkinnedVertex> m_vertices;
        Vector<uint32> m_indices;
    };
}
