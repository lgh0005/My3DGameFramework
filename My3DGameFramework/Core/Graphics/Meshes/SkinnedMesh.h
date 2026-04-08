#pragma once
#include "Graphics/Meshes/Mesh.h"
#include "Graphics/Vertices/Vertex.h"

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
            const Vector<SkinnedVertex>& vertices,
            const Vector<uint32>& indices,
            uint32 primitiveType = GL_TRIANGLES
        );
        virtual void Draw(uint32 count) const override;

    public:
        void ComputeTangents
        (
            Vector<SkinnedVertex>& vertices,
            const Vector<uint32>& indices
        );

    private:
        SkinnedMesh();
        void Init
        (
            const Vector<SkinnedVertex>& vertices,
            const Vector<uint32>& indices,
            uint32 primitiveType
        );
    };
}
