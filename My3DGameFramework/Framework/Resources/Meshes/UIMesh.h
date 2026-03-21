#pragma once
#include "Meshes/Mesh.h"
#include "Descriptors/UIMeshDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(UIMesh)
	class UIMesh : public Mesh
	{
		using Super = Mesh;
		MGF_DECLARE_CUSTOM_ALLOCATORS;

	public:
		static UIMeshPtr Create(UIMeshDescriptor&& desc);
		virtual ~UIMesh() override;

	public:
		virtual bool OnLoad() override;
		virtual bool OnCommit() override;
		virtual void OnRelease() override;

	private:
		UIMesh(UIMeshDescriptor& desc);
		bool Init(UIMeshDescriptor&& desc);

		SVector<UIGeometryData> m_meshData;
	};
}