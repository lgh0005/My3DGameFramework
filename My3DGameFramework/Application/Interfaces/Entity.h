#pragma once
#include "Managers/EntityManager.h"

namespace MGF3D
{
	class Entities
	{
		MGF_DECLARE_UTILITIES(Entities)

	/*============================//
	//     GameObject API         //
	//============================*/
	public:
		static ObjectIDHash Create(const String& name);
		static void Destroy(const String& name);
		static void Destroy(ObjectIDHash id);
		static GameObject* Get(const String& name);
		static GameObject* Get(ObjectIDHash id);

	/*============================//
	//     Component API          //
	//============================*/
	public:
		template<typename T>
		static T* AddComponent(ObjectIDHash ownerID);

		template<typename T>
		static T* GetComponent(ObjectIDHash ownerID);
	};
}

#include "Interfaces/Entity.inl"