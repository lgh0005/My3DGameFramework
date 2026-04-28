#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)

	MGF_CLASS_PTR(Scene)
	class Scene
	{
	public:
		virtual ~Scene();

	/*============================//
	//   MGF3D Scene Custom Type  //
	//============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		bool Init();
		void Update();
		void Clear();

	/*=======================================//
	//   abstract methods for user context   //
	//=======================================*/
	protected:
		Scene();
		virtual bool OnLoadSceneSources()   = 0;
		virtual bool OnPlaceActors()		= 0;
	};
}