#pragma once
#include "Managers/ScriptManager.h"

namespace MGF3D
{
	class Scripts
	{
		MGF_DECLARE_UTILITIES(Scripts)

	/*=================================//
	//    Script instance API          //
	//=================================*/
	public:
		// 특정 엔티티에 스크립트를 생성하여 부착합니다.
		template<typename T>
		static T* AddScript(ObjectIDHash ownerID);

		// 특정 엔티티가 소유한 스크립트를 가져옵니다.
		template<typename T>
		static T* GetScript(ObjectIDHash ownerID);

		// 특정 엔티티의 스크립트를 삭제 대기열에 넣습니다.
		static void Remove(ObjectIDHash ownerID);
	};
}

#include "Interfaces/Scripts.inl"