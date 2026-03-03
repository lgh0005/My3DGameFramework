#pragma once

namespace MGF3D
{
	CLASS_PTR(RenderContext)

	class GlobalUniforms
	{
	public:
		virtual ~GlobalUniforms();
		virtual void PreRender(RenderContext* context) = 0;

	protected:
		GlobalUniforms();
	};
}