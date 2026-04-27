#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(RenderContext)

	class InstanceManager
	{
		MGF_DECLARE_SINGLE(InstanceManager)

	private:
		InstanceManager();
		~InstanceManager();

	public:
		void Extract(RenderContext* context);

	private:
		void ExtractStaticMeshes(RenderContext* context);
		void ExtractSkinnedMeshes(RenderContext* context);
	};
}