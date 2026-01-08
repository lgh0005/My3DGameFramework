#pragma once
#include "Component.h"
#include "Scene/SceneUtils.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Collider)
#pragma endregion

CLASS_PTR(Script)
class Script : public Component
{
public:
	virtual ~Script();
	static const ComponentType s_ComponentType = ComponentType::Script;
	virtual ComponentType GetComponentType() const override { return ComponentType::Script; }

/*============================================//
//  event methods for script : active state   //
//============================================*/
public:
	virtual void OnEnable();
	virtual void OnDisable();

/*=======================================//
//  event methods for script : physics   //
//=======================================*/
public:
	virtual void OnTriggerEnter(Collider* other);
	virtual void OnTriggerStay(Collider* other);
	virtual void OnTriggerExit(Collider* other);

	virtual void OnCollisionEnter(Collider* other);
	virtual void OnCollisionStay(Collider* other);
	virtual void OnCollisionExit(Collider* other);

protected:
	Script();
};