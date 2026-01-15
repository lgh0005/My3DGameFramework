#pragma once
#include "Object/Component.h"
#include "Scene/SceneUtils.h"
#include "Object/ScriptRegistry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Collider)
#pragma endregion

CLASS_PTR(Script)
class Script : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::Script)

public:
	virtual ~Script();
	using ScriptID = uint64;
	virtual ScriptID GetScriptID() const = 0;

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

// TODO : UI와 관련된 상호작용 메서드가 추가될 수 있음
public:

protected:
	Script();
};