#include "EnginePch.h"
#include "LuaBindings.h"

#include "Object/GameObject.h"
#include "Object/Component.h"
#include "Components/Transform.h"
#include "Components/Script.h"

void LuaBindings::Bind(sol::state& lua)
{
    BindMath(lua);
    BindCore(lua);
    BindComponents(lua);
}

void LuaBindings::BindMath(sol::state& lua)
{
    lua.new_usertype<glm::vec3>
    (
        "Vector3",

        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),

        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        
        sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; }
    );
}

void LuaBindings::BindCore(sol::state& lua)
{
    // 1. Component (모든 컴포넌트의 부모)
    lua.new_usertype<Component>
    (
        "Component",
        "GetOwner", [](Component& c) { return c.GetOwner(); }
    );

    // 2. GameObject
    lua.new_usertype<GameObject>
    (
        "GameObject",
        "GetName", [](GameObject& go) { return go.GetName(); },
        "GetTransform", [](GameObject& go) { return &go.GetTransform(); }
    );

    // 3. Script (Component 상속 표시)
    lua.new_usertype<Script>
    (
        "Script",
        sol::base_classes, sol::bases<Component>()
    );
}

void LuaBindings::BindComponents(sol::state& lua)
{
    // Transform (Component 상속 표시)
    lua.new_usertype<Transform>
    (
        "Transform",
        sol::base_classes, sol::bases<Component>(),
        "GetLocalPosition", [](Transform& t) { return t.GetLocalPosition(); },
        "SetLocalPosition", [](Transform& t, const glm::vec3& pos) { t.SetLocalPosition(pos); }
    );
}

