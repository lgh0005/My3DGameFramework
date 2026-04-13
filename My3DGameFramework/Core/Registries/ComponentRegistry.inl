#pragma once

template<typename T>
inline const std::vector<Component*>& ComponentRegistry::GetComponents()
{
	ComponentType type = T::GetStaticComponentType();
	return m_componentCache[(usize)type];
}