#pragma once
#include "Scene/Scene.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Model)

	MGF_CLASS_PTR(SampleScene)
	class SampleScene : public Scene
	{
		using Super = Scene;

	public:
		SampleScene();
		virtual ~SampleScene();

	/*==========================//
	//   SampleScene Custom Type  //
	//==========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*===========================//
	//   SampleScene Main Logic  //
	//===========================*/
	private:
		virtual bool OnLoadSceneSources() override;
		virtual bool OnPlaceActors() override;
	};
}