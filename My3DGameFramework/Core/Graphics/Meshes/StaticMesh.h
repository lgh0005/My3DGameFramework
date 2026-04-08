#pragma once
#include "Graphics/Meshes/Mesh.h"
#include "Graphics/Vertices/Vertex.h"

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
            const Vector<StaticVertex>& vertices,
            const Vector<uint32>& indices,
            uint32 primitiveType = GL_TRIANGLES
        );
        virtual void Draw(uint32 count) const override;

    public:
        void ComputeTangents
        (
            Vector<StaticVertex>& vertices,
            const Vector<uint32>& indices
        );

    private:
        StaticMesh();
        void Init
        (
            const Vector<StaticVertex>& vertices,
            const Vector<uint32>& indices,
            uint32 primitiveType
        );
    };
}