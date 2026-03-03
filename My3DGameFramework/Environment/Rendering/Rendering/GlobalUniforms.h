#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(RenderContext)

	class GlobalUniforms
	{
	public:
		virtual ~GlobalUniforms();
		virtual void PreRender(RenderContext* context) = 0;

	protected:
		GlobalUniforms();
	};
}