#include "EnginePch.h"
#include "Script.h"
#include "Components/Collider.h"
#include "Resources/LuaScript.h"

DECLARE_DEFAULTS_IMPL(Script)

void Script::Bind(LuaScriptPtr scriptResource)
{
	if (!scriptResource) return;

	auto& lua = LUA_MGR.GetLuaVM();

	// 1. 루아 파일 실행 (테이블 반환을 기대)
    sol::protected_function_result result = lua.do_string(scriptResource->GetCode());
    if (!result.valid()) 
    {
        sol::error err = result;
        LOG_ERROR("Lua Script Error: {}", err.what());
        return;
    }

    // 2. 클래스 정의 획득 및 상속 체인 형성
    sol::table scriptClass = result;
    m_self = lua.create_table();
    m_self[sol::metatable_key] = scriptClass;

    // 3. 엔진 객체 바인딩 (루아에서 self.owner 등으로 접근 가능)
    m_self["owner"] = GetOwner();
    m_self["transform"] = &GetTransform();
    m_self["component"] = this; // 스크립트 컴포넌트 자신

    // 4. 라이프사이클 함수 캐싱
    m_awakeFunc = m_self["Awake"];
    m_startFunc = m_self["Start"];
    m_updateFunc = m_self["Update"];
    m_fixedUpdateFunc = m_self["FixedUpdate"];
    m_lateUpdateFunc = m_self["LateUpdate"];
    m_onDestroyFunc = m_self["OnDestroy"];
    m_onEnableFunc = m_self["OnEnable"];
    m_onDisableFunc = m_self["OnDisable"];

    // 5. 물리 이벤트 함수 캐싱
    m_onTriggerEnterFunc = m_self["OnTriggerEnter"];
    m_onTriggerStayFunc = m_self["OnTriggerStay"];
    m_onTriggerExitFunc = m_self["OnTriggerExit"];
    m_onCollisionEnterFunc = m_self["OnCollisionEnter"];
    m_onCollisionStayFunc = m_self["OnCollisionStay"];
    m_onCollisionExitFunc = m_self["OnCollisionExit"];
}

/*==========================================//
//    Life-cycle Mapping Implementation     //
============================================*/
void Script::OnAwake() { InvokeLua(m_awakeFunc, m_self); }
void Script::OnStart() { InvokeLua(m_startFunc, m_self); }
void Script::OnUpdate() { InvokeLua(m_updateFunc, m_self, TIME.GetDeltaTime()); }
void Script::OnFixedUpdate() { InvokeLua(m_fixedUpdateFunc, m_self); }
void Script::OnLateUpdate() { InvokeLua(m_lateUpdateFunc, m_self); }
void Script::OnDestroyed() { InvokeLua(m_onDestroyFunc, m_self); }
void Script::OnEnable() { InvokeLua(m_onEnableFunc, m_self); }
void Script::OnDisable() { InvokeLua(m_onDisableFunc, m_self); }

/*==========================================//
//    Physics & Interaction Events          //
//==========================================*/
void Script::OnTriggerEnter(Collider* other) { InvokeLua(m_onTriggerEnterFunc, m_self, other); }
void Script::OnTriggerStay(Collider* other) { InvokeLua(m_onTriggerStayFunc, m_self, other); }
void Script::OnTriggerExit(Collider* other) { InvokeLua(m_onTriggerExitFunc, m_self, other); }
void Script::OnCollisionEnter(Collider* other) { InvokeLua(m_onCollisionEnterFunc, m_self, other); }
void Script::OnCollisionStay(Collider* other) { InvokeLua(m_onCollisionStayFunc, m_self, other); }
void Script::OnCollisionExit(Collider* other) { InvokeLua(m_onCollisionExitFunc, m_self, other); }

