#pragma once
#include "Object/Component.h"
#include "Scene/SceneUtils.h"
#include <sol/sol.hpp>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Collider)
CLASS_PTR(LuaScript)
#pragma endregion

CLASS_PTR(Script)
class Script : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::Script)

public:
	virtual ~Script();

	static ScriptUPtr Create();
	void Bind(LuaScriptPtr scriptResource);

/*==========================================//
//    Component Life-cycle Overrides        //
//==========================================*/
protected:
	virtual void OnAwake() override;
	virtual void OnStart() override;
	virtual void OnFixedUpdate() override;
	virtual void OnUpdate() override;
	virtual void OnLateUpdate() override;
	virtual void OnDestroyed() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

/*==========================================//
//    Physics & Interaction Events          //
//==========================================*/
public:
	virtual void OnTriggerEnter(Collider* other);
	virtual void OnTriggerStay(Collider* other);
	virtual void OnTriggerExit(Collider* other);

	virtual void OnCollisionEnter(Collider* other);
	virtual void OnCollisionStay(Collider* other);
	virtual void OnCollisionExit(Collider* other);

/*===================================//
//    UI Interaction Events          //
//===================================*/
public:
	// TODO : UI와 관련된 상호작용 메서드가 추가될 수 있음

private:
	Script();

	sol::table m_self;
	sol::function m_awakeFunc;
	sol::function m_startFunc;
	sol::function m_updateFunc;
	sol::function m_fixedUpdateFunc;
	sol::function m_lateUpdateFunc;
	sol::function m_onDestroyFunc;
	sol::function m_onEnableFunc;
	sol::function m_onDisableFunc;

	sol::function m_onTriggerEnterFunc;
	sol::function m_onTriggerStayFunc;
	sol::function m_onTriggerExitFunc;
	sol::function m_onCollisionEnterFunc;
	sol::function m_onCollisionStayFunc;
	sol::function m_onCollisionExitFunc;

	template<typename... Args>
	void InvokeLua(const sol::function& func, Args&&... args);
};

#include "Components/Script.inl"