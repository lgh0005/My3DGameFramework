#pragma once
#include "Pointer/PoolAlloc.h"
#include "Hashing/InstanceIDHash.h"

namespace MGF3D
{
	MGF_CLASS_PTR(ObjectStateMachine);

	MGF_CLASS_PTR(Object)
	class Object : public PoolAlloc
	{
	public:
		virtual ~Object();

	/*================================//
	//      Object State Methods      //
	//================================*/
	public:
		Ptr< ObjectStateMachine> GetState() const;

	/*=============================//
	//      Object ID Methods      //
	//=============================*/
	public:
		InstanceIDHash GetInstanceID() const { return m_instanceID; }
		void SetInstanceID(InstanceIDHash id) { m_instanceID = id; }

	protected:
		Object();
		InstanceIDHash m_instanceID{ 0 };
		ObjectStateMachineUPtr m_stateMachine { nullptr };
	};
}