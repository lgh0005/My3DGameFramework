#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(RenderPipeline)
#pragma endregion

struct Level
{
    std::string        levelName;
    std::string        sceneName;
    std::string        pipelineName;
};

CLASS_PTR(Application)
class Application
{
    using LevelInfo = struct
    {
        std::string sceneKey;
        std::string pipelineKey;
    };

public:
    virtual ~Application();
    virtual void OnInit() = 0;

    void Run(const std::string& startLevelName);
    template<typename T_Scene, typename T_Pipeline>
    void RegisterLevel
    (
        const std::string& levelName,
        const std::string& sceneKey,
        const std::string& pipeKey
    );

protected:
   Application();
    bool Init
    (
        int32 width, 
        int32 height, 
        const std::string& title
    );
    void Update();
    void Render();
    void Shutdown();

private:
    std::unordered_map<std::string, LevelInfo> m_levelMap;
};

#include "Core/Application.inl"