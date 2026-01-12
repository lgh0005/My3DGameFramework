#pragma once

template<typename T>
inline auto ObjectManager::GetComponents() -> const ComponentVector*
{
	// 1. 스크립트인 경우
	if constexpr (std::is_base_of<Script, T>::value)
	{
		ScriptID id = ScriptRegistry::GetID<T>();

		if (id >= m_userScriptComponentCache.size())
			return nullptr;

		// 스크립트 반환
		return &m_userScriptComponentCache[id];
	}

	// 2. 그 외의 일반적인 컴포넌트인 경우
	else
	{
		usize index = static_cast<usize>(T::s_ComponentType);
		return &m_componentCache[index];
	}
}