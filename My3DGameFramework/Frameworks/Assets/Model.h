#pragma once
#include "Sources/Asset.h"
#include "Assets/RawNode.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(Skeleton)

	MGF_CLASS_PTR(Model)
	class Model : public Asset
	{
		MGF_DISABLE_COPY(Model)
		using Super = Asset;

	public:
		virtual ~Model() override;
		static ModelPtr Create(const String& path);

	/*========================//
	//       Model Type       //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		virtual bool Load() override;
		ObjectIDHash Instantiate(const String& name);
		SkeletonPtr GetSkeleton() const { return m_skeleton; }
		const Vector<RawNode>& GetNodes() const { return m_nodes; }

	private:
		Model(const String& path);

		SkeletonPtr m_skeleton{ nullptr };
		Vector<MeshPtr> m_meshes;
		Vector<MaterialPtr> m_materials;
		Vector<RawNode> m_nodes;

		vec3 m_aabbMin{ 0.0f };
		vec3 m_aabbMax{ 0.0f };
	};
}