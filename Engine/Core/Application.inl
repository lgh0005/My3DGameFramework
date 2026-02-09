#pragma once

template<typename T_Scene, typename T_Pipeline>
inline void Application::RegisterLevel
(
    const std::string& levelName,
    const std::string& sceneKey,
    const std::string& pipeKey
)
{
    SCENE.RegisterScene<T_Scene>(sceneKey);
    RENDER.RegisterPipeline<T_Pipeline>(pipeKey);
    m_levelMap[levelName] = { sceneKey, pipeKey };
}