#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(Scene)
    MGF_CLASS_PTR(GameObject)

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
}