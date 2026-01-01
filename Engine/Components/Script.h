#pragma once
#include "Component.h"
#include "Scene/SceneUtils.h"

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
	virtual void OnTriggerEnter();
	virtual void OnTriggerStay();
	virtual void OnTriggerExit();

	virtual void OnCollisionEnter();
	virtual void OnCollisionStay();
	virtual void OnCollisionExit();

protected:
	Script();
};