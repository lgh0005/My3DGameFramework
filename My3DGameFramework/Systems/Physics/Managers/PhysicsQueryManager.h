#pragma once

namespace MGF3D
{
	class PhysicsQueryManager
	{
		MGF_DECLARE_SINGLE(PhysicsQueryManager)

	private:
		PhysicsQueryManager();
		~PhysicsQueryManager();

	public:
		void Init();
		void Shutdown();

	public:
		/*=================================//
		//   예약 실행형 (비동기) API 뼈대   //
		//=================================*/
		// INFO : 여기에서는 우리가 구축해둔 TaskManager를 이용해봄직 함
		// void EnqueueQuery(UniquePtr<IPhysicsQuery> query);
		// ...

	private:
		Ptr<const ::JPH::NarrowPhaseQuery> m_querySystem{ nullptr };
	};
}