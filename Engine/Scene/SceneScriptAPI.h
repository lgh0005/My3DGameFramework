#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Transform)
CLASS_PTR(Component)
#pragma endregion

class SceneScriptAPI
{
public:
    SceneScriptAPI() = default;
    ~SceneScriptAPI() = default;

    /*static GameObject* FindObject(const std::string& name);
    static void Destroy(GameObject* obj);*/

private:
	static Scene* GetCurrentScene();
};

// TODO : Scene 스크립팅 API 추가 고려 필요
// Utils보다는 다른 class를 추가하는 것을 고려해볼 필요가 있다.
// 어렵지 않게 Scene과 friend를 맺게 해서 멤버 접근을 열어두면 됨.
// 씬 스크립팅 API니까 friend 당위성은 충분함.
// 또는, Script 클래스는 Scene 클래스와 friend를 맺어도 됨.