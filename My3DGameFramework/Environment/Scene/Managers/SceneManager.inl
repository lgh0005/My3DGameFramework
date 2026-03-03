#pragma once

template<typename T>
inline void SceneManager::RegisterScene(const std::string& name)
{
	m_scenes[name] = []() -> SceneUPtr
	{
		return T::Create();
	};
}