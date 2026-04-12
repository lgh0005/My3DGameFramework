#pragma once
#include "Scene/Scene.h"

namespace MGF3D
{
	MGF_CLASS_PTR(TestScene)
	class TestScene : public Scene
	{
		using Super = Scene;

	/*==========================//
	//   TestScene Custom Type  //
	//==========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		TestScene();
		virtual ~TestScene();

	private:
		virtual bool OnLoadSceneResources() override;
		virtual bool OnPlaceActors() override;
	};
}