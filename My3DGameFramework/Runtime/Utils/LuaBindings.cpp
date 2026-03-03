#include "EnginePch.h"
#include "LuaBindings.h"

#include "Object/GameObject.h"
#include "Object/Component.h"
#include "Scene/Scene.h"
#include "Components/Transform.h"
#include "Components/Script.h"

LuaBindings::ComponentCache LuaBindings::s_componentGetters;

void LuaBindings::Bind(sol::state& lua)
{
    BindMath(lua);
    BindCore(lua);
    BindComponents(lua);
    BindMangerAPIs(lua);
}

void LuaBindings::BindMath(sol::state& lua)
{
    BindVector2(lua);
    BindVector3(lua);
    BindQuaternion(lua);
}

void LuaBindings::BindCore(sol::state& lua)
{
    // 1. Utils 전역 테이블 생성
    lua.create_named_table
    (
        "Utils",
        
        "Hash", [](const std::string& str) 
        {
            return Utils::StrHash(str);
        }
    );

    BindComponent(lua);
    BindGameObject(lua);
    BindScene(lua);
}

void LuaBindings::BindComponents(sol::state& lua)
{
    BindTransform(lua);
}

void LuaBindings::BindMangerAPIs(sol::state& lua)
{
    BindInput(lua);
}

sol::object LuaBindings::GetComponent(GameObject& go, sol::object type, sol::this_state s)
{
    if (!type.is<sol::table>()) return sol::nil;
    sol::table classTable = type.as<sol::table>();

    // 루아 클래스 테이블에 박혀있는 ID를 읽음
    int32 typeId = classTable.get_or("_TypeId", (int32)ComponentType::Script);

    // CASE 1: 내장 컴포넌트 (Transform, Camera 등)
    if (typeId != (int32)ComponentType::Script)
    {
        // 배열 인덱스 유효성 검사 (안전장치)
        if (typeId >= 0 && typeId < (int32)ComponentType::MAX)
        {
            auto& getter = s_componentGetters[typeId];
            if (getter) return getter(go, s);
        }
    }

    // CASE 2: 유저 스크립트 처리 (Script)
    else
    {
        // 루아 클래스 테이블에 박혀있는 고유 ID(_ClassHash)를 가져옵니다.
        // (Global.lua의 class 함수에서 Utils.Hash("Name")으로 생성된 값)
        uint32 targetHash = classTable.get_or("_ClassHash", 0u);

        // ID가 0이라면 유효하지 않은 클래스이므로 nil 반환
        if (targetHash == 0) return sol::nil;

        // GameObject에 붙은 모든 스크립트를 순회하며 ID 비교
        const auto& scripts = go.GetScripts();
        for (Script* script : scripts)
        {
            // [단순화된 로직] 족보 순회 없이, 정수값 하나만 비교 (매우 빠름)
            if (script && script->GetClassHash() == targetHash)
            {
                // 찾았다면 해당 스크립트 인스턴스를 루아 객체로 포장해서 반환
                return sol::make_object(s, script);
            }
        }
    }

    return sol::nil;
}

void LuaBindings::RegisterComponentGetter(ComponentType type, ComponentGetter getter)
{
    s_componentGetters[(usize)type] = std::move(getter);
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
        sol::call_constructor, sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        
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
        sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),

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
    lua.new_usertype<GameObject>
    (
        "GameObject",

        "Create", &GameObject::Create,

        "GetName", &GameObject::GetName,
        "SetName", &GameObject::SetName,
        "GetInstanceID", &GameObject::GetInstanceID,
        "GetScript", & GameObject::GetScript,

        "SetActive", &GameObject::SetActive,
        "IsActive", &GameObject::IsActive,
        "GetLocalActive", &GameObject::GetLocalActive,
        "SetDestroy", &GameObject::SetDestroy,

        "GetTransform", [](GameObject& go) { return &go.GetTransform(); },
        "GetRoot", &GameObject::GetRoot,
        "SetParent", &GameObject::SetParent,
        "AddChild", &GameObject::AddChild,

        "GetComponent", &LuaBindings::GetComponent
    );
}

void LuaBindings::BindComponent(sol::state& lua)
{
    lua.new_usertype<Component>
    (
        "Component",
        sol::no_constructor, // 추상 클래스이므로 생성 불가

        "GetOwner", &Component::GetOwner,
        "GetTransform", [](Component& c) { return &c.GetTransform(); },

        // 활성화/비활성화
        "enabled", sol::property(&Component::IsEnabled, &Component::SetEnable),
        "isActive", sol::property(&Component::IsActive),
        "IsOwnerActive", &Component::IsOwnerActive
    );
}

void LuaBindings::BindScene(sol::state& lua)
{
    // 1. Scene 클래스 바인딩
    lua.new_usertype<Scene>
    (
        "Scene",
        sol::no_constructor,

        "Destroy", & Scene::Destroy,
        "Find", &Scene::FindObjectByName,
        "FindByID", &Scene::FindObjectByID
    );

    // 2. 전역 접근자
    lua["Scene"]["Current"] = sol::property([]() -> Scene* 
    {
        return SCENE.GetActiveScene();
    });
}

/*================================//
//   Specific component binders   //
//================================*/
void LuaBindings::BindTransform(sol::state& lua)
{
    // 0. Transform 클래스 바인딩
    lua.new_usertype<Transform>
    (
        "Transform",
        sol::base_classes, sol::bases<Component>(), // Component 상속 명시

        // 1. Local Transform 프로퍼티
        // Lua: tr.position = Vector3(10, 0, 0)
        "position", sol::property(&Transform::GetLocalPosition, &Transform::SetLocalPosition),
        "scale", sol::property(&Transform::GetLocalScale, &Transform::SetLocalScale),

        // 회전 (Quaternion) - 엔진 내부 연산용
        // SetRotation이 오버로딩 되어 있으므로, 쿼터니언 버전을 명시적으로 선택(resolve)해야 함
        "rotation", sol::property(&Transform::GetLocalRotation, sol::resolve<void(const glm::quat&)>(&Transform::SetRotation)),

        // [편의기능] 회전 (Euler Angles - Degree) - 사람이 직관적으로 다루기 용
        // Lua: tr.eulerAngles = Vector3(0, 90, 0)
        "eulerAngles", sol::property(&Transform::GetRotationEuler, sol::resolve<void(const glm::vec3&)>(&Transform::SetRotation)),


        // 2. World Transform 프로퍼티
        // Lua: tr.worldPosition = Vector3(...)
        "worldPosition", sol::property(&Transform::GetWorldPosition, &Transform::SetWorldPosition),
        "worldScale", sol::property(&Transform::GetWorldScale, &Transform::SetWorldScale),

        // 월드 회전 (Quaternion & Euler)
        "worldRotation", sol::property(&Transform::GetWorldRotation, sol::resolve<void(const glm::quat&)>(&Transform::SetWorldRotation)),
        "worldEulerAngles", sol::property
        (
            [](Transform& t) { return glm::degrees(glm::eulerAngles(t.GetWorldRotation())); },
            sol::resolve<void(const glm::vec3&)>(&Transform::SetWorldRotation)
        ),


        // 3. 방향 벡터 (Read-Only)
        // Lua: tr.forward * 10
        "forward", sol::property(&Transform::GetForwardVector),
        "up", sol::property(&Transform::GetUpVector),
        "right", sol::property(&Transform::GetRightVector),


        // 4. 이동 및 회전 조작 함수
        "Translate", &Transform::Translate,
        "Rotate", &Transform::Rotate, // Vector3 (Degree) 버전 사용


        // 5. 계층 구조 (Hierarchy)
        // Lua: tr.parent = otherTransform
        "parent", sol::property(&Transform::GetParent, &Transform::SetParent),

        // Lua: local rootGO = tr.root
        "root", sol::property(&Transform::GetRoot),

        // 자식 찾기
        // Lua: local hand = tr:FindChild("RightHand")
        "FindChild", sol::resolve<GameObject*(const std::string&)const>(&Transform::GetChildGameObjectByNameRecursive),

        // 인덱스로 자식 찾기
        "GetChild", &Transform::GetChildGameObjectByIndex,

        // 자식 개수 (Lua 헬퍼)
        "childCount", sol::property([](Transform& t) { return t.GetChildren().size(); })
    );

    // 1. 레지스트리에 겟터 등록
    RegisterComponentGetter
    (
        ComponentType::Transform, 
        [](GameObject& go, sol::this_state s) 
        {
            return sol::make_object(s, &go.GetTransform());
        }
    );

    // 2. 루아 테이블
    lua["Transform"]["_TypeId"] = (int32)ComponentType::Transform;
}

/*=========================//
//   Manager API binders   //
//=========================*/
void LuaBindings::BindInput(sol::state& lua)
{
    // 0. Input 테이블 생성
    auto input = lua.create_named_table("Input");

    // 1. 액션 매핑 & 체크 함수 바인딩
    input.set_function("MapAction", [](const std::string& name, int key) 
    {
        INPUT_MGR.MapAction(name, key);
    });

    input.set_function("MapMouseAction", [](const std::string& name, int btn) 
    {
        INPUT_MGR.MapMouseAction(name, btn);
    });

    input.set_function("GetButton", [](const std::string& name) { return INPUT_MGR.GetButton(name); });
    input.set_function("GetButtonDown", [](const std::string& name) { return INPUT_MGR.GetButtonDown(name); });
    input.set_function("GetButtonUp", [](const std::string& name) { return INPUT_MGR.GetButtonUp(name); });
    input.set_function("GetMousePosition", []() { return INPUT_MGR.GetMousePos(); });

    // 2. Key Enum
    sol::table key = lua.create_table();
    input["Key"] = key;

    // 매크로 정의 (타이핑 노가다 방지)
    #define BIND_KEY(name, code) key[name] = code

   /*========================//
   //   [1] Printable Keys   //
   //========================*/
    BIND_KEY("Space", GLFW_KEY_SPACE);
    BIND_KEY("Apostrophe", GLFW_KEY_APOSTROPHE);
    BIND_KEY("Comma", GLFW_KEY_COMMA);
    BIND_KEY("Minus", GLFW_KEY_MINUS);
    BIND_KEY("Period", GLFW_KEY_PERIOD);
    BIND_KEY("Slash", GLFW_KEY_SLASH);
    BIND_KEY("Semicolon", GLFW_KEY_SEMICOLON);
    BIND_KEY("Equal", GLFW_KEY_EQUAL);
    BIND_KEY("LeftBracket", GLFW_KEY_LEFT_BRACKET);
    BIND_KEY("Backslash", GLFW_KEY_BACKSLASH);
    BIND_KEY("RightBracket", GLFW_KEY_RIGHT_BRACKET);
    BIND_KEY("GraveAccent", GLFW_KEY_GRAVE_ACCENT);
    BIND_KEY("World1", GLFW_KEY_WORLD_1); 
    BIND_KEY("World2", GLFW_KEY_WORLD_2); 

    /*===========================//
    //   [2] Alphanumeric Keys   //
    //===========================*/
    BIND_KEY("Alpha0", GLFW_KEY_0); BIND_KEY("Alpha1", GLFW_KEY_1);
    BIND_KEY("Alpha2", GLFW_KEY_2); BIND_KEY("Alpha3", GLFW_KEY_3);
    BIND_KEY("Alpha4", GLFW_KEY_4); BIND_KEY("Alpha5", GLFW_KEY_5);
    BIND_KEY("Alpha6", GLFW_KEY_6); BIND_KEY("Alpha7", GLFW_KEY_7);
    BIND_KEY("Alpha8", GLFW_KEY_8); BIND_KEY("Alpha9", GLFW_KEY_9);

    BIND_KEY("A", GLFW_KEY_A); BIND_KEY("B", GLFW_KEY_B); BIND_KEY("C", GLFW_KEY_C);
    BIND_KEY("D", GLFW_KEY_D); BIND_KEY("E", GLFW_KEY_E); BIND_KEY("F", GLFW_KEY_F);
    BIND_KEY("G", GLFW_KEY_G); BIND_KEY("H", GLFW_KEY_H); BIND_KEY("I", GLFW_KEY_I);
    BIND_KEY("J", GLFW_KEY_J); BIND_KEY("K", GLFW_KEY_K); BIND_KEY("L", GLFW_KEY_L);
    BIND_KEY("M", GLFW_KEY_M); BIND_KEY("N", GLFW_KEY_N); BIND_KEY("O", GLFW_KEY_O);
    BIND_KEY("P", GLFW_KEY_P); BIND_KEY("Q", GLFW_KEY_Q); BIND_KEY("R", GLFW_KEY_R);
    BIND_KEY("S", GLFW_KEY_S); BIND_KEY("T", GLFW_KEY_T); BIND_KEY("U", GLFW_KEY_U);
    BIND_KEY("V", GLFW_KEY_V); BIND_KEY("W", GLFW_KEY_W); BIND_KEY("X", GLFW_KEY_X);
    BIND_KEY("Y", GLFW_KEY_Y); BIND_KEY("Z", GLFW_KEY_Z);

    /*===========================//
    //   [3] Function Keys       //
    //===========================*/
    BIND_KEY("Esc", GLFW_KEY_ESCAPE);
    BIND_KEY("Enter", GLFW_KEY_ENTER);
    BIND_KEY("Tab", GLFW_KEY_TAB);
    BIND_KEY("Backspace", GLFW_KEY_BACKSPACE);
    BIND_KEY("Insert", GLFW_KEY_INSERT);
    BIND_KEY("Delete", GLFW_KEY_DELETE);
    BIND_KEY("Right", GLFW_KEY_RIGHT);
    BIND_KEY("Left", GLFW_KEY_LEFT);
    BIND_KEY("Down", GLFW_KEY_DOWN);
    BIND_KEY("Up", GLFW_KEY_UP);
    BIND_KEY("PageUp", GLFW_KEY_PAGE_UP);
    BIND_KEY("PageDown", GLFW_KEY_PAGE_DOWN);
    BIND_KEY("Home", GLFW_KEY_HOME);
    BIND_KEY("End", GLFW_KEY_END);
    BIND_KEY("CapsLock", GLFW_KEY_CAPS_LOCK);
    BIND_KEY("ScrollLock", GLFW_KEY_SCROLL_LOCK);
    BIND_KEY("NumLock", GLFW_KEY_NUM_LOCK);
    BIND_KEY("PrintScreen", GLFW_KEY_PRINT_SCREEN);
    BIND_KEY("Pause", GLFW_KEY_PAUSE);

    BIND_KEY("F1", GLFW_KEY_F1);   BIND_KEY("F2", GLFW_KEY_F2);
    BIND_KEY("F3", GLFW_KEY_F3);   BIND_KEY("F4", GLFW_KEY_F4);
    BIND_KEY("F5", GLFW_KEY_F5);   BIND_KEY("F6", GLFW_KEY_F6);
    BIND_KEY("F7", GLFW_KEY_F7);   BIND_KEY("F8", GLFW_KEY_F8);
    BIND_KEY("F9", GLFW_KEY_F9);   BIND_KEY("F10", GLFW_KEY_F10);
    BIND_KEY("F11", GLFW_KEY_F11); BIND_KEY("F12", GLFW_KEY_F12);

    /*==============================//
    //   [4] Keypad Keys (NumPad)   //
    //==============================*/
    BIND_KEY("Keypad0", GLFW_KEY_KP_0); BIND_KEY("Keypad1", GLFW_KEY_KP_1);
    BIND_KEY("Keypad2", GLFW_KEY_KP_2); BIND_KEY("Keypad3", GLFW_KEY_KP_3);
    BIND_KEY("Keypad4", GLFW_KEY_KP_4); BIND_KEY("Keypad5", GLFW_KEY_KP_5);
    BIND_KEY("Keypad6", GLFW_KEY_KP_6); BIND_KEY("Keypad7", GLFW_KEY_KP_7);
    BIND_KEY("Keypad8", GLFW_KEY_KP_8); BIND_KEY("Keypad9", GLFW_KEY_KP_9);

    BIND_KEY("KeypadDecimal", GLFW_KEY_KP_DECIMAL);
    BIND_KEY("KeypadDivide", GLFW_KEY_KP_DIVIDE);
    BIND_KEY("KeypadMultiply", GLFW_KEY_KP_MULTIPLY);
    BIND_KEY("KeypadSubtract", GLFW_KEY_KP_SUBTRACT);
    BIND_KEY("KeypadAdd", GLFW_KEY_KP_ADD);
    BIND_KEY("KeypadEnter", GLFW_KEY_KP_ENTER);
    BIND_KEY("KeypadEqual", GLFW_KEY_KP_EQUAL);

    /*=======================//
    //   [5] Modifier Keys   //
    //=======================*/
    BIND_KEY("LeftShift", GLFW_KEY_LEFT_SHIFT);
    BIND_KEY("LeftCtrl", GLFW_KEY_LEFT_CONTROL);
    BIND_KEY("LeftAlt", GLFW_KEY_LEFT_ALT);
    BIND_KEY("LeftSuper", GLFW_KEY_LEFT_SUPER);
    BIND_KEY("RightShift", GLFW_KEY_RIGHT_SHIFT);
    BIND_KEY("RightCtrl", GLFW_KEY_RIGHT_CONTROL);
    BIND_KEY("RightAlt", GLFW_KEY_RIGHT_ALT);
    BIND_KEY("RightSuper", GLFW_KEY_RIGHT_SUPER);
    BIND_KEY("Menu", GLFW_KEY_MENU);

    #undef BIND_KEY

    // 3. Mouse Enum
    sol::table mouse = lua.create_table();
    mouse["Left"] = GLFW_MOUSE_BUTTON_LEFT;
    mouse["Right"] = GLFW_MOUSE_BUTTON_RIGHT;
    mouse["Middle"] = GLFW_MOUSE_BUTTON_MIDDLE;
    input["Mouse"] = mouse;
}