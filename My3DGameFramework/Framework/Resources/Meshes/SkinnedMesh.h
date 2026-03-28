#pragma once
#include "Meshes/Mesh.h"
#include "Descriptors/SkinnedMeshDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(SkinnedMesh)
	class SkinnedMesh : public Mesh
	{
		using Super = Mesh;

	/*==================================//
	//         Type System              //
	//==================================*/
	public:
		static UniquePtr<MGFType> s_type;
		static void InitializeType();
		virtual const MGFType* GetType() const override;

	public:
		static SkinnedMeshPtr Create(SkinnedMeshDescriptor&& desc);
		virtual ~SkinnedMesh() override;

	public:
		virtual bool OnLoad() override;
		virtual bool OnCommit() override;
		virtual void OnRelease() override;

	private:
		SkinnedMesh(SkinnedMeshDescriptor& desc);
		bool Init(SkinnedMeshDescriptor&& desc);
		
		void ComputeTangents();
		SVector<SkinnedGeometryData> m_meshData;
	};
}