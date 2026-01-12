#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(GameObject)
#pragma endregion

class SceneUtils
{
    DECLARE_STATIC_CLASS(SceneUtils)

public:
    // static GameObject* FindObject(const std::string& name);
    static void Destroy(GameObject* obj);
    // TODO : static GameObject* Instantiate();

private:
    static Scene* GetCurrentScene();
};
