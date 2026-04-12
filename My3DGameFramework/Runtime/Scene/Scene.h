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

	/*======================================//
	//   default scene GameObject methods   //
	//======================================*/
	public:
		bool Init();
		// void AddGameObject(GameObjectUPtr gameObject);
		// void Destroy(GameObject* obj);

	/*======================================//
	//   default scene life-cycle methods   //
	//======================================*/
	public:
		void FixedUpdate();
		void Update();
		void LateUpdate();

	/*=======================================//
	//   abstract methods for user context   //
	//=======================================*/
	protected:
		Scene();
		virtual bool OnLoadSceneResources() = 0;
		virtual bool OnPlaceActors()		= 0;

	protected:
		// TODO : 게임 오브젝트와 컴포넌트 레지스트리
	};
}