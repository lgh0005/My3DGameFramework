#pragma once
#include "Scene/Scene.h"

namespace MGF3D
{
	MGF_CLASS_PTR(TestScene)
	class TestScene : public Scene
	{
		using Super = Scene;

	public:
		TestScene();
		virtual ~TestScene();

	/*==========================//
	//   TestScene Custom Type  //
	//==========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*=========================//
	//   TestScene Main Logic  //
	//=========================*/
	private:
		virtual bool OnLoadSceneSources() override;
		virtual bool OnPlaceActors() override;
	};
}