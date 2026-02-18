#pragma once
#include <sol/sol.hpp>

class LuaBindings
{
	DECLARE_STATIC_CLASS(LuaBindings)

public:
	static void Bind(sol::state& lua);

	static void BindMath(sol::state& lua);
	static void BindCore(sol::state& lua);

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

};