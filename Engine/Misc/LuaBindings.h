#pragma once
#include <sol/sol.hpp>
#include "Misc/Enums.h"

class LuaBindings
{
	DECLARE_STATIC_CLASS(LuaBindings)
	using ComponentGetter = std::function<sol::object(GameObject&, sol::this_state)>;
	using ComponentCache = std::array<ComponentGetter, (usize)ComponentType::MAX>;

private:
	static ComponentCache s_componentGetters;
	static void RegisterComponentGetter(ComponentType type, ComponentGetter getter);

public:
	static void Bind(sol::state& lua);
	static void BindMath(sol::state& lua);
	static void BindCore(sol::state& lua);
	static void BindComponents(sol::state& lua);
	static void BindMangerAPIs(sol::state& lua);
	static sol::object GetComponent
	(
		GameObject& go,
		sol::object type,
		sol::this_state s
	);

/*==================//
//   Math binders   //
//==================*/
private:
	static void BindVector2(sol::state& lua);
	static void BindVector3(sol::state& lua);
	static void BindQuaternion(sol::state& lua);

/*====================//
//   Object binders   //
//====================*/
private:
	static void BindGameObject(sol::state& lua);
	static void BindComponent(sol::state& lua);
	static void BindScene(sol::state& lua);

/*================================//
//   Specific component binders   //
//================================*/
private:
	static void BindTransform(sol::state& lua);

/*=========================//
//   Manager API binders   //
//=========================*/
private:
	static void BindInput(sol::state& lua);
};