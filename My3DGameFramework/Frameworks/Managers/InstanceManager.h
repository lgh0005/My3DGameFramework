#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Camera)
	MGF_CLASS_PTR(RenderContext)

	class InstanceManager
	{
		MGF_DECLARE_SINGLE(InstanceManager)

	private:
		InstanceManager();
		~InstanceManager();

	public:
		void Extract(RenderContext* context, const Camera* camera);

	private:
		void ExtractStaticMeshes(RenderContext* context, const Camera* camera);
		void ExtractSkinnedMeshes(RenderContext* context, const Camera* camera);
		void ExtractOutlineMeshes(RenderContext* context, const Camera* camera);
	};
}