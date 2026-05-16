#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GUIPass)
	class GUIPass : public RenderPass
	{
	public:
		virtual ~GUIPass();
		static GUIPassUPtr Create
		(
			int32 width = WINDOW_WIDTH,
			int32 height = WINDOW_HEIGHT
		);

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		GUIPass();
		bool Init(int32 width, int32 height);

		// TODO : ImGUI 윈도우 인스턴스 추가

	};
}