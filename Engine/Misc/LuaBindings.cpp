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
}

void LuaBindings::BindMath(sol::state& lua)
{
    BindVector2(lua);
    BindVector3(lua);
    BindQuaternion(lua);
}

void LuaBindings::BindCore(sol::state& lua)
{
    BindComponent(lua);
    BindGameObject(lua);
    BindScene(lua);
}

/*==================//
//   Math binders   //
//==================*/
void LuaBindings::BindVector2(sol::state& lua)
{
    lua.new_usertype<glm::vec2>
    (
        // 전역 객체 이름
        "Vector2",
        
        // 생성자
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        
        // 프로퍼티
        "x", sol::property([](glm::vec2& v) { return v.x; }, [](glm::vec2& v, float val) { v.x = val; }),
        "y", sol::property([](glm::vec2& v) { return v.y; }, [](glm::vec2& v, float val) { v.y = val; }),

        // 연산자 오버로딩
        sol::meta_function::addition, [](const glm::vec2& a, const glm::vec2& b) { return a + b; },
        sol::meta_function::subtraction, [](const glm::vec2& a, const glm::vec2& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload
        (
            [](const glm::vec2& v, float s) { return v * s; },
            [](float s, const glm::vec2& v) { return s * v; },
            [](const glm::vec2& a, const glm::vec2& b) { return a * b; }
        ),
        sol::meta_function::division, [](const glm::vec2& v, float s) { return v / s; },
        sol::meta_function::unary_minus, [](const glm::vec2& v) { return -v; },

        // 메서드
        "Length", [](const glm::vec2& v) { return glm::length(v); },
        "LengthSq", [](const glm::vec2& v) { return glm::length2(v); },
        "Normalize", [](const glm::vec2& v) { return glm::normalize(v); },
        "Distance", [](const glm::vec2& a, const glm::vec2& b) { return glm::distance(a, b); },
        "Dot", [](const glm::vec2& a, const glm::vec2& b) { return glm::dot(a, b); },
        "Lerp", [](const glm::vec2& a, const glm::vec2& b, float t) { return glm::lerp(a, b, t); }
    );
}

void LuaBindings::BindVector3(sol::state& lua)
{
    lua.new_usertype<glm::vec3>
    (
        // 전역 객체 이름
        "Vector3",

        // 생성자 
        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),

        // 프로퍼티
        "x", sol::property([](glm::vec3& v) { return v.x; }, [](glm::vec3& v, float val) { v.x = val; }),
        "y", sol::property([](glm::vec3& v) { return v.y; }, [](glm::vec3& v, float val) { v.y = val; }),
        "z", sol::property([](glm::vec3& v) { return v.z; }, [](glm::vec3& v, float val) { v.z = val; }),

        // 연산자 오버로딩
        sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
        sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload
        (
            [](const glm::vec3& v, float s) { return v * s; },
            [](float s, const glm::vec3& v) { return s * v; },
            [](const glm::vec3& a, const glm::vec3& b) { return a * b; }
        ),
        sol::meta_function::division, [](const glm::vec3& v, float s) { return v / s; },
        sol::meta_function::unary_minus, [](const glm::vec3& v) { return -v; },

        // 메서드
        "Length", [](const glm::vec3& v) { return glm::length(v); },
        "LengthSq", [](const glm::vec3& v) { return glm::length2(v); },
        "Normalize", [](const glm::vec3& v) { return glm::normalize(v); },
        "Distance", [](const glm::vec3& a, const glm::vec3& b) { return glm::distance(a, b); },
        "Dot", [](const glm::vec3& a, const glm::vec3& b) { return glm::dot(a, b); },
        "Cross", [](const glm::vec3& a, const glm::vec3& b) { return glm::cross(a, b); },
        "Lerp", [](const glm::vec3& a, const glm::vec3& b, float t) { return glm::lerp(a, b, t); }
    );
}

void LuaBindings::BindQuaternion(sol::state& lua)
{
    lua.new_usertype<glm::quat>
    (
        // 전역 객체 이름
        "Quaternion",
        
        // 생성자
        sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),

        // 프로퍼티
        "x", sol::property([](glm::quat& q) { return q.x; }, [](glm::quat& q, float v) { q.x = v; }),
        "y", sol::property([](glm::quat& q) { return q.y; }, [](glm::quat& q, float v) { q.y = v; }),
        "z", sol::property([](glm::quat& q) { return q.z; }, [](glm::quat& q, float v) { q.z = v; }),
        "w", sol::property([](glm::quat& q) { return q.w; }, [](glm::quat& q, float v) { q.w = v; }),

        // 정적 생성 함수 (Static Functions)
        "FromEuler", [](float x, float y, float z) 
        {
            return glm::quat(glm::vec3(glm::radians(x), glm::radians(y), glm::radians(z)));
        },
        "Identity", []() { return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); },

        // 산술 연산 (곱셈)
        sol::meta_function::multiplication, sol::overload
        (
            [](const glm::quat& a, const glm::quat& b) { return a * b; }, // 회전 합치기
            [](const glm::quat& q, const glm::vec3& v) { return q * v; }  // 벡터 회전
        ),

        // 메서드
        "Inverse", [](const glm::quat& q) { return glm::inverse(q); },
        "Slerp", [](const glm::quat& a, const glm::quat& b, float t) { return glm::slerp(a, b, t); },
        "ToEuler", [](const glm::quat& q) 
        {
            glm::vec3 euler = glm::eulerAngles(q);
            return glm::vec3(glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z));
        },
        "Length", [](const glm::quat& q) { return glm::length(q); },
        "Normalize", [](const glm::quat& q) { return glm::normalize(q); }
    );
}

/*====================//
//   Object binders   //
//====================*/
void LuaBindings::BindGameObject(sol::state& lua)
{

}

void LuaBindings::BindComponent(sol::state& lua)
{

}

void LuaBindings::BindScene(sol::state& lua)
{

}

/*================================//
//   Specific component binders   //
//================================*/
void LuaBindings::BindTransform(sol::state& lua)
{

}
