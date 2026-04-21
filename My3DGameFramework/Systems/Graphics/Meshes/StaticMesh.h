#pragma once
#include "Meshes/Mesh.h"
#include "Vertices/StaticVertex.h"

namespace MGF3D
{
    MGF_CLASS_PTR(StaticMesh)
    class StaticMesh : public Mesh
    {
        using Super = Mesh;

    public:
        virtual ~StaticMesh() override;
        static StaticMeshPtr Create
        (
            Vector<StaticVertex>&& vertices,
            Vector<uint32>&&       indices,
            uint32 primitiveType = GL_TRIANGLES
        );

        virtual bool OnSyncCreate() override;
        virtual void Draw(uint32 count) const override;

    /*=====================//
    //   StaticMesh Type   //
    //=====================*/
    public:
        static int16 s_typeIndex;
        virtual const MGFType* GetType() const override;

    private:
        StaticMesh();
  
        Vector<StaticVertex> m_vertices;
        Vector<uint32> m_indices;
    };
}