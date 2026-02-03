#pragma once

//template<typename T>
//inline void InputManager::BindAction(const std::string& actionName, T* instance, void(T::* func)())
//{
//	auto boundFunc = [instance, func]() { (instance->*func)(); };
//	GetOrCreateAction(actionName)->Callbacks[(int32)eventType].push_back(boundFunc);
//}