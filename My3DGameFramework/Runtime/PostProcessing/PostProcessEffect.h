#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(RenderContext)
	MGF_CLASS_PTR(GLFramebuffer2D)

	MGF_CLASS_PTR(PostProcessEffect)
	class PostProcessEffect
	{
	public:
		virtual ~PostProcessEffect();
		
	public:
		virtual bool Render(RenderContext* context) = 0;
		virtual void Resize(int32 width, int32 height) = 0;

	public:
		bool IsEfectEnabled() const;
		void SetEffectEnabled(bool enabled);

	protected:
		PostProcessEffect();
		bool m_isEffectEnable { true };
	};
}